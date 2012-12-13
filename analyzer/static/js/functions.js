

//var draw_data;

var x, y;

var num_ticks = 15;

var prev_x_pos = 0.0;

//var on_x_ax_left = false;
//var on_x_ax_right = false;
var zoom_left = false;
var zoom_right = false;
var pan = false;

var min_x, max_x;

var min_width = 1;
var min_dist = min_width;

var loaded_data;

var chart;
var chart_width;
var chart_height;

var padding = 10;

var bottom_space = 20;

$(document).ready(function(){
	
	$("#overlay").hide();
	
	max_width = Math.max.apply(null, [$("#devices").width(), $("#kernels").width()]);
	$("#devices").width(max_width);
	$("#kernels").width(max_width);
/*
	$("#data").width(
		window.innerWidth - $("#data").position().left - 20	
	);
*/	
	$("#data_content").height(
		//window.innerHeight - $("#experiments_title").outerHeight() - $("#experiments").outerHeight()
		window.innerHeight - $("#data_content").position().top - 10
	);
	
	// Resize the chart
	chart_width = $("#data_content").width();
	chart_height = $("#data_content").height();
	
	chart = d3.select("#data_content").append("svg")
		.attr("class", "chart")
		.attr("id", "data_plot")
		.attr("width", chart_width)
		.attr("height", chart_height)
		.on("mousedown", function(d) {
			
			rel_mouse_position = (d3.svg.mouse(chart[0][0])[0] / chart_width);
			
			if (rel_mouse_position < 0.4) {
				zoom_left = true;
			} else if (rel_mouse_position > 0.6) {
				zoom_right = true;
			} else if (rel_mouse_position > 0.45 && rel_mouse_position < 0.55) {
				pan = true;
			};
		});

	$(".entry").click(function(){
		$(this).toggleClass("selected");
		data_update();
	});
	
});

function data_update() {
	
	selected_kernel_ids = [];
	selected_device_ids = [];
	
	$.each($(".entry.selected"), function(index, value) {
	
		id = $(value).html().split(":", 1);
		
		if ($(value).hasClass("kernel")) {
			selected_kernel_ids += id;
		}
		
		if ($(value).hasClass("device")) {
			selected_device_ids += id;
		}
	
	});
	
	if ((selected_kernel_ids.length != 0) || (selected_device_ids.length != 0)) {
		
		show_overlay();
		
		// Trigger request to DB
		$.getJSON('/get_data', {
			devices: selected_device_ids,
			kernels: selected_kernel_ids,
			start_time: 0,
			stop_time: 0
		}, function(data) {	
			
			// Set the global variable
			loaded_data = data.result;
			
			min_x = d3.min(loaded_data.map(function(value,index) { return value[1]; }));
			max_x = d3.max(loaded_data.map(function(value,index) { return value[2]; }));
			
			min_threads = d3.min(loaded_data.map(function(value,index) { return value[0]; }));
			max_threads = d3.max(loaded_data.map(function(value,index) { return value[0]; }));			
		
			x = d3.scale.linear()
				.domain([0, max_x - min_x])
				.range([10, chart_width - 10]);
		
			y = d3.scale.linear()
				.domain([min_threads - 1, max_threads + 1])
			    .rangeRound([0, chart_height]);
			
			plot()
			
			hide_overlay();
			
		});
		
	}
}

function plot() {

	// Preselect values	
	filtered_data = loaded_data.filter(function(elem) {

		if (elem[2] - min_x < x.domain()[0] || elem[1] - min_x > x.domain()[1]) {
			return false;
		}
		
		return true;
		
	});
	
	// Convert to drawing range
	filtered_data.forEach(function(value, index, array) {
		array[index] = [y(value[0] - 0.5), x(value[1] - min_x), x(value[2] - min_x)];
	});
	
	// Remove duplicates
	//var this_draw_data = filtered_data;
	var this_draw_data = [];

	var cur_thread = NaN
	var prev_end = NaN
	
	filtered_data.forEach(function(value, index, array) {
		
		if (cur_thread != value[0]) {
			cur_thread = value[0];
			
			this_draw_data.push(value);
			prev_end = value[2];
		
			return;
		}
		
		
		// Calculate the distance between start of
		// value and end of previous value
		dist = value[1] - prev_end;
		
		// If the distance is below threshold, set
		// this end as new end, if not, push the
		// current value into the array
		if (dist < min_dist) {
			this_draw_data[this_draw_data.length - 1][2] = value[2];
		} else {
			this_draw_data.push(value);
		}

		prev_end = value[2];
		
	});
	
	console.log(this_draw_data.length);
	
	// Draw the grid
	chart.selectAll("#vgrid")
	.remove();

	chart.selectAll("lines")
		.data(x.ticks(num_ticks))
		.enter().append("line")
			.attr("x1", x)
			.attr("x2", x)
			.attr("y1", 0)
			.attr("y2", chart_height - bottom_space)
			.attr("id", "vgrid")
			.style("stroke", "#bbb")
			.style("stroke-width", "1px")
			.style("stroke-dasharray", "5,5");
	
	// Place all bars
	rects = chart.selectAll("rect")
	rects.remove();
	
	bar_height = y(1) - y(0);
	
	chart.selectAll("rect")
		.data(this_draw_data)
		.enter().append("rect")
			.attr("y", function(d, i) { return d[0]; })
			.attr("x", function(d, i) { return d[1]; })
			.attr("width", function(d, i) {
				width = Math.abs((d[2] - d[1]));
				if (width < min_width) { return min_width; }
				return width;
			})
			.attr("height", bar_height - 5);
	
	chart.selectAll("#labels")
		.remove();
	
	chart.selectAll(".rule")
		.data(x.ticks(num_ticks))
		.enter().append("text")
			.attr("class", "rule")
			.attr("id", "labels")
			.attr("x", x)
			.attr("y", chart_height)
			.attr("dy", -3)
			.attr("text-anchor", "middle")
			.attr("fill", "#fff")
			.text(function(value) { return d3.round(value, 9); });

}

d3.select('body')
	.on("mousemove", function(d) {
		
		var old_x;
		
		if (zoom_left || zoom_right || pan) {
			
			old_x = x.domain();
			
		} else {
			
			prev_x_pos = d3.svg.mouse(chart[0][0])[0];
			return;
			
		}
		
		//granularity = Math.log(1 / ((max_x - min_x) / (old_x[1] - old_x[0]) * 100) + 1) / Math.log(10); 
		
		//console.log(granularity)
		
		console.log((old_x[1] - old_x[0]) / (max_x - min_x));
		
		new_x1 = old_x[0] - (d3.svg.mouse(chart[0][0])[0] - prev_x_pos) * 0.1;
		new_x2 = old_x[1] - (d3.svg.mouse(chart[0][0])[0] - prev_x_pos) * 0.1;
	
		if (zoom_left) {
			if (new_x1 > -0.1) {
				x.domain([new_x1, old_x[1]]);
			}
		}
		
		else if	(zoom_right) { x.domain([old_x[0], new_x2]);   }
		
		else if	(pan) {
			if (new_x1 > -0.1) {
				x.domain([new_x1, new_x2]);
			}
		}
		
		prev_x_pos = d3.svg.mouse(chart[0][0])[0];

		plot();
		
	})
	
	.on("mouseup", function(d) {
		zoom_left = false;
		zoom_right = false;
		pan = false;
	});
