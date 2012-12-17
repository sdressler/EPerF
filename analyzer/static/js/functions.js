

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
var left_space = 250;

var rel_mouse_position = 0;

var num_experiments_to_load;

var prev_x_position = NaN;

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
	
	static_plot();
	
}

function add_mouse_events() {
	
	$("#data_content")
		.mousemove(function(event) {
			
			rel_mouse_position = (event.pageX - left_space) / chart_width;
			
			if (zoom_left == false && zoom_right == false && pan == false) {
				
				if (rel_mouse_position > 0.0 && rel_mouse_position < 0.4) {
					$("body").css('cursor', 'ew-resize');
					$("#footer").html("Drag to zoom left");
				} else if (rel_mouse_position > 0.6) {
					$("body").css('cursor', 'ew-resize');
					$("#footer").html("Drag to zoom right");
				} else if (rel_mouse_position > 0.45 && rel_mouse_position < 0.55) {
					$("body").css('cursor', 'ew-resize');
					$("#footer").html("Drag to pan");
				} else {
					$("body").css('cursor', 'auto');
					$("#footer").html("");
				};
				
			} else if (zoom_left || true && zoom_right || true && pan || true) {
				$("body").css('cursor', 'move');
				
				old_x = x.domain();
				inv_zoom_level = (old_x[1] - old_x[0]) / max_x;
				
				delta = (event.pageX - prev_x_position) * max_x * 0.01 * inv_zoom_level;
				
				if (!isNaN(delta)) {
					
					if (zoom_left == true) {
						x.domain([old_x[0] - delta, old_x[1]]);
					} else if (zoom_right == true) {
						x.domain([old_x[0], old_x[1] - delta]);
					} else if (pan == true) {
						x.domain([old_x[0] - delta, old_x[1] - delta]);
					}
					
					plot();
				}
				
				prev_x_position = event.pageX;
				
			} else {
				$("body").css('cursor', 'pointer');
			}
		})
		
		.mousedown(function(event) {
			
			event.originalEvent.preventDefault();
			
			if (rel_mouse_position > 0.0 & rel_mouse_position < 0.4) {
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
			$("#footer").html("");
		})
		
		.mouseup(function(event) {
			zoom_left = false;
			zoom_right = false;
			pan = false;
			prev_x_position = NaN;
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

			static_plot();
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
		.range([left_space + 10, chart_width - 20]);

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
		
		prev_end_index = this_draw_data.length - 1;
		prev_end = NaN;
		
		// For later
		start_index = prev_end_index + 1; 
		
		filtered_data.forEach(function(value, index, array) {
			
			this_start = x(value[0]);
			
			//console.log(this_start);
			
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
		
		//console.log(this_draw_data.length);
		
		// Set correct start, if needed
		if ((this_draw_data.length > 0) && (this_draw_data[start_index][0] < left_space + 10)) {
			this_draw_data[start_index][0] = left_space + 10;
		}
		
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
			.attr("width", function(d) {
				width = d[1] - d[0];
				if (width < min_width) { return min_width; }
				return width;
			})
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
	
}

var call_text = function(d) {
	if (d > 1) {
		return " Calls";
	} else {
		return " Call";
	}
}

function static_plot() {
	
	if (typeof db_data == "undefined") { return; }
	
	key_list = Object.keys(db_data).sort();
	
	var text_data = [];
	
	key_list.forEach(function (key, key_index) {
		text_data.push([
		    key_index,
		    0,
		    db_data[key].length + "x"
		]);
		text_data.push([
            key_index,
            1,
            d3.round(db_data[key][0][0], 9) + " s to " +
		    d3.round(db_data[key][db_data[key].length - 1][1], 9) + " s"
		]);
		text_data.push([
		    key_index,
		    2,
		    "Duration: " + d3.round(db_data[key][db_data[key].length - 1][1] - db_data[key][0][0], 9) + " s"
		]);
		text_data.push([
            key_index,
            3,
            "Median: " + d3.round(d3.median(db_data[key].map(function(d) { return d[1] - d[0]; })), 9) + " s"
		])
	});
	
	chart.selectAll(".stats").remove();
	
	chart.selectAll(".stats")
		.data(text_data)
		.enter().append("text")
			.attr("class", "stats")
			.attr("x", 5)
			.attr("y",  function(d) { return y(d[0]) + 11; })
			.attr("dy", function(d) { return d[1] * 18; })
			.attr("text-anchor", "left")
			.attr("alignment-baseline", "middle")
			.attr("fill", "#fff")
			.text(function(d) { return d[2]; });
	
}
