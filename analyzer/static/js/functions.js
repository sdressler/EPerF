

//var draw_data;

var x;
var y;

var num_ticks = 10;

var prev_x_pos = 0.0;

var on_x_ax_left = false;
var on_x_ax_right = false;

var min_x, max_x;
var min_width = 5;

var bottom_border = 20;

var loaded_data;

//var chart = d3.select("#data_svg")
/*.append("svg")
	.attr("class", "chart")
	.attr("id", "data_plot")
	.attr("width", 500)
	.attr("height", 500)
//	.attr("width", window.innerWidth - $("data").position().left)
//	.attr("height", window.innerHeight)
	//.attr("height", window.innerHeight - ($("#data").position().top + $("#data").outerHeight()))
*/
/*
	.on("mousedown", function(d) {
		if ((d3.svg.mouse(chart[0][0])[0] / chart_width) < 0.5) {
			on_x_ax_left = true;
		} else {
			on_x_ax_right = true;
		};
	});
*/

var chart;

var chart_width; // = chart.attr("width");
var chart_height; // = chart.attr("height") - bottom_border;

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
	chart_width = $("#data_content").innerWidth();
	chart_height = $("#data_conent").innerHeight();
	
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
			
			x = d3.scale.linear()
			 .domain([min_x, max_x])
		     .range([0, chart_width]);
		
			y = d3.scale.ordinal()
				.domain([
					d3.min(loaded_data.map(function(value,index) { return value[0]; })),
				    d3.max(loaded_data.map(function(value,index) { return value[0] + 1; }))		 
			    ])
			    .rangeBands([0, chart_height / 3]);
			
			plot()
			
		});
		
	}
}

function plot() {

	//var chart = d3.select("#data_plot");
	
	//this_draw_data = data.result;
	
	// Preselect values
	this_draw_data = loaded_data.filter(function(elem) {

		if (elem[2] < x.domain()[0] || elem[1] > x.domain()[1]) {
			return false;
		}
		
		return true;
		
	});
	
	
	//this_draw_data = loaded_data;
	
	//console.log(this_draw_data.length);
	//console.log(this_draw_data);
/*	
	chart.selectAll("#grid")
	.remove();

	chart.selectAll("lines")
		.data(x.ticks(num_ticks))
		.enter().append("line")
			.attr("x1", x)
			.attr("x2", x)
			.attr("y1", 0)
			.attr("y2", chart_height - bottom_border)
			.attr("id", "grid")
			.style("stroke", "#000")
			.style("stroke-width", "1px")
			.style("stroke-dasharray", "5,5");
	
	rects = chart.selectAll("rect")
	rects.remove();
	
	chart.selectAll("rect")
		.data(this_draw_data)
		.enter().append("rect")
			.attr("y", function(d, i) { return y.rangeBand() * (d[0] + 1); })
			.attr("x", function(d, i) { return x(d[1]); })
			.attr("width", function(d, i) {
				width = Math.abs(parseFloat(x(d[2])) - parseFloat(x(d[1])));
				if (width < min_width) {
					return min_width;
				}
				return width;
			})
			.attr("height", y.rangeBand());
			//.attr("fill-opacity", 0.9);
	
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
			.text(String);
*/	
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
