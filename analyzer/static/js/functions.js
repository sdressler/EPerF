$(document).ready(function(){
	
	max_width = Math.max.apply(null, [$("#devices").width(), $("#kernels").width()]);
	$("#devices").width(max_width);
	$("#kernels").width(max_width);
/*
	$("#result_data").width(
		$(window).width() - $("#result_data").offset().left - 20	
	);
*/	
	$(".entry").click(function(){
		$(this).toggleClass("selected");
		
		// Trigger a data update
		data_update();
		
	});
	
	$("#plot_btn").click(plot());
	
});

//var draw_data;

var x;
var y;

var num_ticks = 10;

var prev_x_pos = 0.0;

var on_x_ax_left = false;
var on_x_ax_right = false;

var min_x, max_x;

var bottom_border = 30;

var loaded_data;

var chart = d3.select("body").append("svg")
	.attr("class", "chart")
	.attr("id", "data_plot")
	.attr("width", $(window).width() - 10)
	.attr("padding-left", "5px")
	.attr("padding-right", "5px")
	.attr("height", 200);

var chart_width = chart.attr("width");
var chart_height = chart.attr("height") - bottom_border;

chart.append("line")
	.attr("x1", 0)
	.attr("x2", chart_width)
	.attr("y1", chart_height)
	.attr("y2", chart_height)
	.style("stroke", "#000")
	.style("stroke-width", "8px")
	//.append("svg:title")
	//.text("Move axis for zoom.")
	.on("mousedown", function(d) {
		if ((d3.svg.mouse(chart[0][0])[0] / chart_width) < 0.5) {
			on_x_ax_left = true;
		} else {
			on_x_ax_right = true;
		};
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
			
			table = '<table border="1"><tr>';
			table += '<td>#</td><td>TID</td><td>Start [s]</td><td>Stop [s]</td>';
			table += '</tr>';
			
			$.each(data.result, function(i, row) {			
				table += '<tr>';
				table += '<td>' + i + '</td><td>' + row[0] + '</td><td>' + row[1] + '</td><td>' + row[2] + '</td>';  
				table += '</tr>';
			});
			
			loaded_data = data.result;
				
			table += '</tr></table>'
				
			$("#data_table").html(table);

			var draw_data = data.result;
			
			min_x = d3.min(draw_data.map(function(value,index) { return value[1]; }));
			max_x = d3.max(draw_data.map(function(value,index) { return value[2]; })); 
			
			x = d3.scale.linear()
		     .domain([min_x, min_x + 5])
		     .range([0, chart_width]);
		
			y = d3.scale.ordinal()
				.domain([
					d3.min(draw_data.map(function(value,index) { return value[0]; })),
				    d3.max(draw_data.map(function(value,index) { return value[0] + 1; }))		 
			    ])
			    .rangeBands([0, chart_height / 3]);
			
			plot()
			
		});
		
	}
}

function plot() {
	
	// min/max range must be available
	if (typeof min_x == 'undefined') {
		return;
	}
	
	// Load partial data from within the domain
	// Trigger request to DB
	/*
	$.getJSON('/get_data', {
		devices: selected_device_ids,
		kernels: selected_kernel_ids,
		start_time: x.domain()[0],
		stop_time: x.domain()[1]
	}, function(data) {	
	*/
		var chart = d3.select("#data_plot");
		
		//this_draw_data = data.result;
		this_draw_data = loaded_data.filter(function(elem) {
			return elem[1] > x.domain()[0] && elem[2] < x.domain()[1];
		});
		
		console.log(this_draw_data.length);
		
		return;
		
		rects = chart.selectAll("rect")
		rects.remove();
		
		chart.selectAll("rect")
			.data(this_draw_data)
			.enter().append("rect")
				.attr("y", function(d, i) { return y.rangeBand() * (d[0] + 1); })
				.attr("x", function(d, i) { return x(d[1]); })
				.attr("width", function(d, i) { return Math.abs(parseFloat(x(d[2])) - parseFloat(x(d[1]))); })
				.attr("height", y.rangeBand());
		
		chart.selectAll("#grid")
			.remove();
		
		chart.selectAll("lines")
			.data(x.ticks(num_ticks))
			.enter().append("line")
				.attr("x1", x)
				.attr("x2", x)
				.attr("y1", 0)
				.attr("y2", chart_height)
				.attr("id", "grid")
				.style("stroke", "#000")
				.style("stroke-width", "1px");
		
		chart.selectAll("#labels")
			.remove();
		
		chart.selectAll(".rule")
			.data(x.ticks(num_ticks))
			.enter().append("text")
				.attr("class", "rule")
				.attr("id", "labels")
				.attr("x", x)
				.attr("y", chart_height + bottom_border)
				.attr("dy", -3)
				.attr("text-anchor", "middle")
				.text(String);
	//
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
			
			new_x = old_x[0] - (d3.svg.mouse(chart[0][0])[0] - prev_x_pos) * 0.05;
			x.domain([new_x, old_x[1]]);
			
		}
		
		if (on_x_ax_right) {
			
			new_x = old_x[1] - (d3.svg.mouse(chart[0][0])[0] - prev_x_pos) * 0.05;
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
