

//var draw_data;

var x, y;

var num_ticks = 20;

var prev_x_pos = 0.0;

var zoom_left = false;
var zoom_right = false;
var pan = false;

//var min_x = NaN;
var max_x = NaN;

var min_width = 1;
var min_dist = min_width;

var loaded_data;
var colors_data;

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
	hide_footer(0.1);
	
	load_experiments();
});

$(window).resize(function() {
	resize_data_content();
	update_scales();
	plot();
});

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
			open: function(hex, rgba) {
				set_selector_color(this, hex);
			},
			change: function(hex, rgba) {
				set_selector_color(this, hex);
				plot();
			},
			close: function(hex, rgba) {
				//plot
			}
		})
	});
}

function set_selector_color(trigger, color) {
	selector = $(trigger).attr('id').slice(6);
	$('#' + selector).css('color', color);
}

function load_experiments() {
	
	var e = $("#experiments");
	
	show_overlay();
	
	var experiments = [];
	
	$.ajax({
	    type: 'GET',
	    url: '/get_experiments',
	    dataType: 'json',
	    data: {},
	    async: true,
	    success: function(data) {
	    	
	    	num_experiments_to_load = data.result.length - 1;
	    	
	    	data.result.forEach(function(value) {
	    		experiments.push(value[0]);
	    		e.append('\
	    		    <div class="container" id="' + value[0] + '">\
	    		        <div class="entry experiment">' + value[0] +'</div>\
	    		        <div class="clear"></div> \
	    		    </div>');
	    		
	    		load_experiment_detail(value[0]);
	    		
	    	});
	    	
		    //load_experiments_finalize();

			
	    }
	});
		
}

function load_experiment_detail(experiment) {

	$.ajax({
	    type: 'GET',
	    url: '/get_experiment_overview',
	    dataType: 'json',
	    data: { id: experiment },
	    async: false,
	    success: function(data) {
	    	
	    	var ex = $("#" + experiment);
	    	
	    	ex.append('<div class="entry eheader">K:</div>');
    		data.result[0].forEach(function(entry) {
    			ex.append(    					
    			    '<div class="' + experiment + ' kernel entry"> \
    			    	 <div style="float: left;"><input class="color-picker" type="hidden" value="#B0BC00" id="color_k_' + entry[0] + '"></div> \
    			    	 <div class="selectable" kid="' + entry[0] + '" id="k_' + entry[0] + '">'
    			         	+ '&nbsp; ' + entry[0] + ':&nbsp;' + entry[1] +
    			         '</div> \
    			     </div>'
    			);
    		});
/*	    		
	    		ex.append('<div class="clear"></div>');
	    		
	    		ex.append('<div class="entry eheader">D:</div>');
	    		data.result[1].forEach(function(entry) {
	    			ex.append(    					
	    			    '<div class="' + experiment + ' device entry"> \
	    			    	 <div style="float: left;"><input class="color-picker" type="hidden" value="#B0BC00" id="color_d_' + entry[0] + '"></div> \
	    			    	 <div class="selectable" did="' + entry[0] + '" id="d_' + entry[0] + '">'
	    			         	+ '&nbsp; ' + entry[0] + ':&nbsp;' + entry[1] +
	    			         '</div> \
	    			     </div>'
	    			);
	    		});
*/		    
			
			//console.log("finalized?");
			//hide_overlay();
    		
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
		
		if ($(this).hasClass("selected")) {
			// Already selected
			$(this).removeClass("selected");
			
			$('#color_' + event.target.id).miniColors('disabled', true);
			$(this).css('color', '#fff');
		} else {
			// Not yet selected
			$(this).addClass("selected");
			
			$('#color_' + event.target.id).miniColors('disabled', false);
			$(this).css('color', $('#color_' + event.target.id).attr('value'));
		}

		fetch_data_from_db_for_selections();
		
	});
		
	console.log("finished");
	hide_overlay();
}

function fetch_data_from_db_for_selections() {
	
	var k = [];
	var d = [];
	
	var e = {};
	
	$(".selected").each(function(idx, elem) {
		
		var experiment = elem.parentElement.classList[0];
		var type = elem.parentElement.classList[1];

		e[experiment] = '';
		
		if (type == "kernel") {
			k.push($(elem).attr('kid'));
			k.push(experiment);
		} else if (type == "device") {
			d.push($(elem).attr('did'));
			d.push(experiment);
		} else {
			console.error("Error: Unknown Type.");
		}
		
	});
	
	if (d.length == 0 && k.length == 0) {
		clear_plot();
		return;
	}
	
	show_overlay();
	
	// Data request
	$.ajax({
	    type: 'POST',
	    url: '/get_data',
	    dataType: 'json',
	    data: { d: d, k: k, e: Object.keys(e) },
	    async: false,
	    success: function(data) {
	    
	    	// Initialize map
	    	loaded_data = {};
	    	colors_data = {};
	    	
	    	//loaded_data = data.result;
			data.result.forEach(function(value, index, array) {
				
				key = value[3] + "-" + String(value[4]) + "-" + String(value[5]) + "-" + String(value[0]);
				
				if (typeof loaded_data[key] == 'undefined') {
					loaded_data[key] = [];
					colors_data[key] = value[4];
				}
				
				loaded_data[key].push([value[0], value[1], value[2]]);
				
			});
			
			//min_x = d3.min(data.result.map(function(value,index) { return value[1]; }));
			max_x = d3.max(data.result.map(function(value,index) { return value[2]; }));
			
			update_scales();
			plot();
	    	
	    }
	});
	
}

function update_scales() {
	
	// Only if data is available
	if (typeof loaded_data == "undefined") { return; }
	
	//console.log([min_x, max_x]);
	
	x = d3.scale.linear()
		.domain([0, max_x])
		.range([10, chart_width - 10]);

	y = d3.scale.linear()
		//.domain([min_threads - 1, max_threads + 1])
		.domain([0, Object.keys(loaded_data).length])
	    .rangeRound([bottom_space - 10, chart_height - bottom_space - 10]);
	
	
	add_mouse_events();
	
}

function plot() {
	
	// Only if data is available
	if (typeof loaded_data == "undefined") { return; }
	
	// Preselect values
	filtered_data = {};
	
	keys = Object.keys(loaded_data);
	
	keys.forEach(function(key) {
		loaded_data[key].forEach(function(value) {
			if (typeof filtered_data[key] == 'undefined') {
				filtered_data[key] = [];
			}
			
			if (value[2] > x.domain()[0] || value[2] < x.domain()[1]) {
				filtered_data[key].push(value);
			}
		});
	});
		
	// Convert to drawing range
	n = 0;
	keys.forEach(function(key) {
		filtered_data[key].forEach(function(value, index, array) {
			array[index] = [y(n), x(value[1]), x(value[2])];
		});
		n++;
	});
	
	//console.log(filtered_data);
	
	// Remove duplicates
	//var this_draw_data = filtered_data;
	var this_draw_data = [];

	var cur_key = ""
	var prev_end = NaN
	
	keys.forEach(function(key) {
		
		filtered_data[key].forEach(function(value, index, array) {
			
			if (cur_key != key) {
				cur_key = key;
				
				value.push(colors_data[key]);
				this_draw_data.push(value);
				
				prev_end = value[2];
				
				return;
			}
			
			// Calculate the distance between start of
			// value and end of previous value
			dist = value[1] - prev_end;
			
			// If the distance is below threshold, set
			// a new end, if not, push the
			// current value into the array
			if (dist < min_dist) {
				this_draw_data[this_draw_data.length - 1][2] = value[2];
			} else {
				value.push(colors_data[key]);
				this_draw_data.push(value);
			}

			prev_end = value[2];
			
		});
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
			.attr("y", function(d, i) { return d[0]; })
			.attr("x", function(d, i) { return d[1]; })
			.attr("width", function(d, i) {
				width = Math.abs((d[2] - d[1]));
				if (width < min_width) { return min_width; }
				return width;
			})
			.attr("height", bar_height - 5)
			.attr("fill", function(d, i) { return $('#color_k_' +  d[3]).attr('value'); });
	
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
		
		var old_x;
		
		if (zoom_left || zoom_right || pan) {
			
			old_x = x.domain();
			
		} else {
			
			prev_x_pos = d3.svg.mouse(chart[0][0])[0];
			return;
			
		}
		
		zoom_level = (old_x[1] - old_x[0]) / max_x; 
		
		new_x1 = old_x[0] - (d3.svg.mouse(chart[0][0])[0] - prev_x_pos) * zoom_level * 0.0001;
		new_x2 = old_x[1] - (d3.svg.mouse(chart[0][0])[0] - prev_x_pos) * zoom_level * 0.0001;
		
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
