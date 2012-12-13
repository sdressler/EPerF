

//var draw_data;

var x, y;

var num_ticks = 10;

var prev_x_pos = 0.0;

var on_x_ax_left = false;
var on_x_ax_right = false;

var min_x, max_x;
var min_width = 5;

var loaded_data;

var chart;
var chart_width;
var chart_height;

var padding = 10;

var bottom_space = 20;

$(document).ready(function(){
	
	max_width = Math.max.apply(null, [$("#devices").width(), $("#kernels").width()]);
	$("#devices").width(max_width);
	$("#kernels").width(max_width);

	$("#data").width(
		window.innerWidth - $("#data").position().left - 20	
	);
	
	$("#data_content").height(
		window.innerHeight -$("#data").position().top - 100 	
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
			if ((d3.svg.mouse(chart[0][0])[0] / chart_width) < 0.5) {
				on_x_ax_left = true;
			} else {
				on_x_ax_right = true;
			};
		});

	$(".entry").click(function(){
		$(this).toggleClass("selected");
		
		// Trigger a data update
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
				.domain([min_x, max_x])
				.range([0, chart_width]);
				//.rangeRound([0, chart_width]);
		
			y = d3.scale.linear()
				.domain([min_threads - 1, max_threads + 1])
			    .rangeRound([0, chart_height]);
			
			plot()
			
		});
		
	}
}

function plot() {

	// Preselect values	
	filtered_data = loaded_data.filter(function(elem) {

		if (elem[2] < x.domain()[0] || elem[1] > x.domain()[1]) {
			return false;
		}
		
		return true;
		
	});
	
	// Convert to drawing range
	filtered_data.forEach(function(value, index, array) {
		array[index] = [y(value[0] - 0.5), x(value[1]), x(value[2])];
	});
	
	// Remove duplicates
	var this_draw_data = filtered_data;
	var prev_value = NaN;
	/*
	filtered_data.forEach(function(elem, index, array) {
		if (prev_value != d3.round(elem[1], 2)) {
			this_draw_data.push(elem);
		}
		
		prev_value = d3.round(elem[1], 2);
	});
	*/
	/*
	this_draw_data = filtered_data.filter(function(elem, index, array) {
		
		if (index > 0) {
			prev_elem = array[index - 1];
			if (prev_elem[1] == elem[1]) {
				return false;
			}
		}
		
		return true;
		
	});
	*/
	
	//this_draw_data = filtered_data;
	
	console.log(this_draw_data.length);
	//console.log(this_draw_data);
	
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
			.text(String);

}

d3.select('body')
	.on("mousemove", function(d) {
		
		
		var old_x;
		
		if (on_x_ax_left || on_x_ax_right) {
			old_x = x.domain();
		} else {
			prev_x_pos = d3.svg.mouse(chart[0][0])[0];
			return
		}
		
		if (on_x_ax_left) {
			
			new_x = old_x[0] - (d3.svg.mouse(chart[0][0])[0] - prev_x_pos) * 0.001;
			x.domain([new_x, old_x[1]]);
			
		}
		
		if (on_x_ax_right) {
			
			new_x = old_x[1] - (d3.svg.mouse(chart[0][0])[0] - prev_x_pos) * 0.001;
			x.domain([old_x[0], new_x]);
			
		}
		
		prev_x_pos = d3.svg.mouse(chart[0][0])[0];

		plot();
		
	})
	
	.on("mouseup", function(d) {
		//console.log("mouse up");
		on_x_ax_left = false;
		on_x_ax_right = false;
	});
