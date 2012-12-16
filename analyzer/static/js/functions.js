

//var draw_data;

var x, y;

var num_ticks = 20;

var prev_x_pos = 0.0;

var zoom_left = false;
var zoom_right = false;
var pan = false;

var max_x = NaN;

var min_width = 2;
var min_dist = min_width;

var db_data;
var loaded_data;
var colors_per_key = {};

var chart;
var chart_width;
var chart_height;

var padding = 10;

var bottom_space = 30;

var rel_mouse_position = 0;

var num_experiments_to_load;

/*
 * Initial routines
 */
$(document).ready(function(){
	//$("#overlay").hide();

	$("#db_entries").css('top', window.innerHeight / 2 - $("#db_entries").height() / 2);

	$("#db_entries").each(function (index, value) {
		$(this).click(function(event) { select_db($(event.target).html()); });
	});
		
	resize_data_content();
	hide_footer(0.1);
	
	//select_db("sleep.db");
	//select_db("octree_multiple.db");
	
});

$(window).resize(function() {
	resize_data_content();
	update_scales();
	plot();
});

function select_db(db) {	
	$("#db_selector").fadeOut();
	load_experiments(db);
}

/*
 * 
 * This function is necessary in order to make the plotting SVG bigger without
 * oversizing the window
 * 
 */
function resize_data_content() {
	
	$("#data_content").height(
		window.innerHeight - $("#data_content").offset().top - $("#footer").outerHeight()
	);
	
	$("#footer")
		.width(window.innerWidth)
		.css('top', window.innerHeight - $("#footer").outerHeight() - 10);
	
	// Resize the chart
	chart_width = $("#data_content").width();
	chart_height = $("#data_content").height();
	
	d3.select("#data_content_svg")
		.remove();
	
	chart = d3.select("#data_content").append("svg:svg")	
		.attr("class", "chart")
		.attr("id", "data_content_svg")
		.attr("width", chart_width)
		.attr("height", chart_height);
	
}

function add_mouse_events() {
	
	$("#data_content")
		.mousemove(function(event) {
			
			rel_mouse_position = event.pageX / chart_width;
			
			if (zoom_left == false && zoom_right == false && pan == false) {
				if (rel_mouse_position < 0.4) {
					$("body").css('cursor', 'ew-resize');
				} else if (rel_mouse_position > 0.6) {
					$("body").css('cursor', 'ew-resize');
				} else if (rel_mouse_position > 0.45 && rel_mouse_position < 0.55) {
					$("body").css('cursor', 'ew-resize');
				} else {
					$("body").css('cursor', 'auto');
				};
			} else if (zoom_left || true && zoom_right || true && pan || true) {
				$("body").css('cursor', 'move');
			} else {
				$("body").css('cursor', 'pointer');
			}
		})
		
		.mousedown(function(event) {
			
			event.originalEvent.preventDefault();
			
			if (rel_mouse_position < 0.4) {
				zoom_left = true;
			} else if (rel_mouse_position > 0.6) {
				zoom_right = true;
			} else if (rel_mouse_position > 0.45 && rel_mouse_position < 0.55) {
				pan = true;
			} else {
				zoom_left = false;
				zoom_right = false;
				pan = false;
				$("body").css('cursor', 'auto');
			};
		})
		
		.mouseout(function(event) {
			$("body").css("cursor", "auto");
		});

}

function clear_plot() {
	
	// Remove mouse events
	$("#data_content")
		.unbind('mousemove')
		.unbind('mousedown');
	
	// Plot cleanup
	chart.selectAll("#vgrid").remove();
	chart.selectAll("rect").remove();
	chart.selectAll("#labels").remove();
	
	// Footer aways
	hide_footer();
	
}

function create_color_pickers() {
	
	$('.color-picker').each(function(index, value) {
		$(this).miniColors({
			disabled: true,
			change: function(hex, rgba) {
				$($(this).parent().children()[2]).css('color', hex);
				colors_per_key[$(this).attr('id')] = hex;
				plot();
			}
		})
	});
}

function load_experiments(db) {
	
	var e = $("#experiments");
	
	show_overlay();
	
	var experiments = [];
	
	$.ajax({
	    type: 'GET',
	    url: '/get_experiments',
	    dataType: 'json',
	    data: { db: db },
	    async: true,
	    success: function(data) {
	    	
	    	num_experiments_to_load = data.result.length - 1;
	    	
	    	data.result.forEach(function(value) {
	    		
	    		experiments.push(value[0]);
	    		
	    		var date = new Date(value[1] * 1000);
	    		
	    		e.append('\
	    		    <div class="container experiment" id="' + value[0] + '">\
	    		        <div class="entry experiment title">'
	    		    		+ '<div style="font-weight: bold">' + value[2] + '</div>'
	    		    		+ '<div>' + date.toString() + '</div> \
	    		        </div> \
	    		    	<div class="clear"></div> \
	    		    </div>');
	    		
	    		load_experiment_detail(value[0]);
	    		
	    	});
	    	
	    }
	});
		
}

function load_experiment_detail(experiment) {

	$.ajax({
	    type: 'GET',
	    url: '/get_experiment_overview',
	    dataType: 'json',
	    data: { id: experiment },
	    async: true,
	    success: function(data) {
	    	
	    	var ex = $("#" + experiment);
	    	
	    	data.result.forEach(function(entry) {
	    		ex.append(
	    			'<div>' +
	    				'<input class="color-picker" type="hidden" value="#B0BC00" id="color_' + experiment + '-' + entry[0] + '-' + entry[2] + '">' +
	    				'<div class="entry selectable" kid="' + entry[0] + '" did="' + entry[2] + '">'
	    					+ entry[1] + ' - ' + entry[3] +
	    				'</div>' +
	    				'<div class="clear"></div>' +
	    			'</div>'
	    		);
    		});
    		
    		if (num_experiments_to_load == 0) {
    			load_experiments_finalize();
    		} else {
    			num_experiments_to_load--;
    		}
    		
	    }
	    
	});
	
}
	
function load_experiments_finalize() {
	
	resize_data_content();
	create_color_pickers();
	
	// Make them all selectable
	$(".selectable").click(function(event) {
		
		var color_input = $(this).parent().children()[0];
		var color = color_input.value;
	
		colors_per_key[color_input.id] = color; 
		
		if ($(this).hasClass("selected")) {
			
			// Already selected
			$(this).removeClass("selected");
			$(this).css('color', '#fff');
			
			$(color_input).miniColors('disabled', true);
			
		} else {
			
			// Not yet selected
			$(this).addClass("selected");
			$(this).css('color', color);
			
			$(color_input).miniColors('disabled', false);
		}

		fetch_data_from_db_for_selections();
		
	});
	
	resize_data_content();
	
	hide_overlay();
}

function fetch_data_from_db_for_selections() {

	var selection = [];
	
	$(".selected").each(function(idx, elem) {
		
		var experiment = $(elem).parent().parent().attr('id');
		var kernel = $(elem).attr('kid');
		var device = $(elem).attr('did');
		
		selection.push([experiment, kernel, device]);

	});
	
	if (selection.length == 0) {
		clear_plot();
		return;
	}
	
	show_overlay();
	
	// Data request
	$.ajax({
	    type: 'POST',
	    url: '/get_data',
	    dataType: 'json',
	    data: { sel: selection },
	    async: true,
	    success: function(data) {

	    	db_data = data.result;
	    	max = [];
	    	
	    	// Get the maximum value
	    	Object.keys(db_data).forEach(function(key, index, array) {
	    		max.push(
	    			d3.max(db_data[key].map(function(value, index) { return value[1]; }))
	    		)
	    	});
	    	
	    	max_x = d3.max(max);
	    	
			update_scales();

			plot();
			
			hide_overlay();
	    	
	    }
	});
	
}

function update_scales() {
	
	// Only if data is available
	if (typeof db_data == "undefined") { return; }
	
	x = d3.scale.linear()
		.domain([0, max_x])
		.range([10, chart_width - 10]);

	y = d3.scale.linear()
		.domain([0, Object.keys(db_data).length])
	    .rangeRound([bottom_space - 20, chart_height - bottom_space - 20]);
	
	add_mouse_events();
	
}

function plot() {
	
	// Only if data is available
	if (typeof db_data == "undefined") { return; }
	
	key_list = Object.keys(db_data).sort();
	
	//filtered_data = {};
	this_draw_data = [];
	
	// Preselect to match current plotting range
	key_list.forEach(function(key, key_index) {
		
		filtered_data = [];
		indices = [];
		
		db_data[key].forEach(function(value, index, array) {
			
			if (value[1] > x.domain()[0] && value[0] < x.domain()[1]) {
				filtered_data.push(value);
				indices.push(index);
			}
			
		});
		
		prev_end_index = 0;
		prev_end = NaN;
		
		filtered_data.forEach(function(value, index, array) {
			
			this_start = x(value[0]);
			this_end = x(value[1]);
			
			dist = this_start - prev_end;
			
			if (dist < min_dist) {
				this_draw_data[prev_end_index][1] = this_end;
			} else {
				this_draw_data.push([
				    this_start, // Start
				    this_end,   // Stop
				    y(key_index), // y Position
				    colors_per_key['color_' + key.slice(0, key.length - 2)] // Color
				]);
			}
			
			prev_end = this_end;
			prev_end_index = this_draw_data.length - 1;
			
		})
		
	});

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
			.attr("y",     function(d) { return d[2];        })
			.attr("x",     function(d) { return d[0];        })
			.attr("width", function(d) { return d[1] - d[0]; })
			.attr("fill",  function(d) { return d[3];        })
			.attr("height", bar_height - 5);
	
	chart.selectAll("#labels")
		.remove();
	
	chart.selectAll(".rule")
		.data(x.ticks(num_ticks))
		.enter().append("text")
			.attr("class", "rule")
			.attr("id", "labels")
			.attr("x", x)
			.attr("y", chart_height - 10)
			.attr("dy", -3)
			.attr("text-anchor", "middle")
			.attr("fill", "#fff")
			.text(function(value) { return d3.round(value, 9); });
	
	show_footer();
	
}

d3.select('body')
	.on("mousemove", function(d) {
		
		if (typeof chart == "undefined") {
			return;
		}
		
		var old_x;
		
		if (zoom_left || zoom_right || pan) {
			
			old_x = x.domain();
			
		} else {
			
			prev_x_pos = d3.svg.mouse(chart[0][0])[0];
			return;
			
		}
		
		zoom_level = (old_x[1] - old_x[0]) / max_x;
		
		new_x1 = old_x[0] - (d3.svg.mouse(chart[0][0])[0] - prev_x_pos) * zoom_level * 0.01 * max_x;
		new_x2 = old_x[1] - (d3.svg.mouse(chart[0][0])[0] - prev_x_pos) * zoom_level * 0.01 * max_x;
		
		if (zoom_left) {			
			x.domain([new_x1, old_x[1]]);
		}
		
		else if	(zoom_right) { x.domain([old_x[0], new_x2]);   }
		
		else if	(pan) {
			x.domain([new_x1, new_x2]);
		}
		
		prev_x_pos = d3.svg.mouse(chart[0][0])[0];

		plot();
		
	})
	
	.on("mouseup", function(d) {
		zoom_left = false;
		zoom_right = false;
		pan = false;
		$("body").css('cursor', 'auto');
	});
