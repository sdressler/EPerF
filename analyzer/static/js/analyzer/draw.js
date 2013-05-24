
/*
 * 
 * This function is necessary in order to make the plotting SVG bigger without
 * oversizing the window
 * 
 */
function resize_data_content() {
	
	$("#chart_container")
		.width(window.innerWidth - $("#statistics").width())
		.height(window.innerHeight -
				$("#chart_container").offset().top -
				$("#footer").outerHeight() - 10
		 )
		.css('left', $("#statistics").width() + 25)
		.css('top', $("#statistics").position().top + 55);
	
	$("#statistics_content")
		.height($("#chart_container").height())
		.css('top', $("#statistics").css('top'));
	
	$("#footer")
		.width(window.innerWidth)
		.css('top', window.innerHeight - $("#footer").outerHeight() - 10);
	
	// Resize the chart
	chart_width = $("#chart_container").width();
	chart_height = $("#chart_container").height();
	
	d3.select("#chart_svg")
		.remove();
	
	chart = d3.select("#chart_container").append("svg:svg")	
		.attr("class", "chart")
		.attr("id", "chart_svg")
		.attr("width", "100%") //chart_width)
		.attr("height", "100%"); //chart_height);
	
	static_plot();
	
}

$("#chart_container")
	.mouseup(function(event) {
		zoom_left = false;
		zoom_right = false;
		pan = false;
		prev_x_position = NaN;
		$("body").css('cursor', 'auto');
	})
	
	.mouseout(function(event) {
		zoom_left = false;
		zoom_right = false;
		pan = false;
		prev_x_position = NaN;
		hide_circle();
		$("body").css('cursor', 'auto');
	})
	
	.mousemove(function(event) {
		
		rel_mouse_position = event.pageX / chart_width;
		
		//if (event.pageY > $("#data_content").position().top && $(".miniColors-selector").length == 0) {
		if (true) {
		
			if (!plot_empty && !zoom_left && !zoom_right && !pan) {
				
				if (rel_mouse_position > 0.0 && rel_mouse_position < 0.4) {
					$("body").css('cursor', 'ew-resize');
					show_circle("Drag to zoom");
					circle_follow(event);
				} else if (rel_mouse_position > 0.6) {
					$("body").css('cursor', 'ew-resize');
					show_circle("Drag to zoom");
					circle_follow(event);
				} else if (rel_mouse_position > 0.45 && rel_mouse_position < 0.55) {
					$("body").css('cursor', 'ew-resize');
					show_circle("Drag to pan");
					circle_follow(event);
				} else {
					$("body").css('cursor', 'auto');
					hide_circle();
				};
				
			} else if (zoom_left || true && zoom_right || true && pan || true) {
				$("body").css('cursor', 'move');
				
				if (typeof x == "undefined") { return; }
				
				old_x = x.domain();
				inv_zoom_level = (old_x[1] - old_x[0]) / max_x;
				
				delta = (event.pageX - prev_x_position) * max_x * 0.01 * inv_zoom_level;
				
				if (!isNaN(delta)) {
                        
                    e1 = old_x[0] - delta;
                    e2 = old_x[1] - delta;
					
					if (zoom_left == true) {
                        
                        if (e1 < 0) {
    						x.domain([0, old_x[1]]);
                        } else {
                            x.domain([e1, old_x[1]]);
                        }

					} else if (zoom_right == true) {

                        if (e2 > max_x) {
						    x.domain([old_x[0], max_x]);
                        } else {
                            x.domain([old_x[0], e2]);
                        }
    
					} else if (pan == true) {

                        if (e1 > 0 && e2 < max_x) {
						    x.domain([e1, e2]);
                        }

                        if (e1 < 0) {
                            x.domain([0,old_x[1]]);
                        }

                        if (e2 > max_x) {
                            x.domain([old_x[0], max_x]);
                        }
					}
					
					plot();
				}
				
				prev_x_position = event.pageX;
				
			} else {
				$("body").css('cursor', 'auto');
				hide_circle();
			}
		
		} else {
			$("body").css('cursor', 'auto');
			hide_circle();
			zoom_left = false;
			zoom_right = false;
			pan = false;
			prev_x_position = NaN;
		}
	})
	
	.mousedown(function(event) {
		
		event.originalEvent.preventDefault();
		
		hide_circle();
		
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

function clear_plot() {
	
	// Remove mouse events
	$("#data_content")
		.unbind('mousemove')
		.unbind('mousedown');
	
	// Plot cleanup
	chart.selectAll("#vgrid").remove();
	chart.selectAll("rect").remove();
	chart.selectAll("#labels").remove();
	
	plot_empty = true;
	
}

function update_scales() {
	
	// Only if data is available
	if (typeof db_data == "undefined") { return; }
	
	x = d3.scale.linear()
		.domain([0, max_x])
		.rangeRound([10, chart_width - 50]);

	y = d3.scale.linear()
		.domain([0, Object.keys(db_data).length])
	    .rangeRound([bottom_space - 20, chart_height - bottom_space - 20]);
	
}

var bisect_l = d3.bisector(function(d) { return d[0]; }).left;
var bisect_r = d3.bisector(function(d) { return d[1]; }).right;

function plot() {

	// Only if data is available
	if (typeof db_data == "undefined") { return; }
   
    var draw_data = {}
    var key_index = {}
    var idx = 0
	// Preselect to match current plotting range
    $.each(db_data, function(key, obj) {

        //lo = d3.bisectLeft(db_data[key], x.domain()[0]);
        //hi = d3.bisectRight(db_data[key], x.domain()[1]);

        lo = bisect_l(db_data[key], x.domain()[0]);
        hi = bisect_r(db_data[key], x.domain()[1]);

        /* This merges elements that are to close to each other */
        draw_data[key] = [];

        if (lo != hi) {
            draw_data[key].push([x(db_data[key][lo][0]),x(db_data[key][lo][1])]);
            var start, stop;
            var j = 0;
            for (var i = lo + 1; i < hi; i++) {

                start = x(db_data[key][i][0]);
                stop  = x(db_data[key][i][1]);

                if ((start - draw_data[key][j][1]) < min_width) {
                    draw_data[key][j][1] = stop;
                } else {
                    draw_data[key].push([start,stop]);
                    j++;
                }
            }
        }
        key_index[key] = y(idx);
        idx++;
		
	});

    // Cleanup everything
    chart.selectAll(".drawings").remove();

	// Draw the grid
	chart.selectAll("lines")
		.data(x.ticks(num_ticks))
		.enter().append("line")
			.attr("class", "drawings")
			.attr("x1", x)
			.attr("x2", x)
			.attr("y1", 0)
			.attr("y2", chart_height - bottom_space)
			.attr("id", "vgrid")
			.style("stroke", "#bbb")
			.style("stroke-width", "1px")
			.style("stroke-dasharray", "5,5");
	
	bar_height = y(1) - y(0);

    $.each(draw_data, function(key,value) {
        chart.selectAll(".rect-" + key)
            .data(value)
            .enter().append("rect")
                .attr("class", "drawings rect-" + key)
                .attr("y", function(d) { return key_index[key]; })
                .attr("x", function(d) { return d[0]; })
                .attr("width", function(d) {
                    w = d[1] - d[0];
                    
                    if (w < min_width) { return min_width; }

                    return w;
                })
                .attr("height", bar_height - 5);

        chart.selectAll(".rect-" + key).attr("fill", function() {
            ckey = key.split("-");
            return colors_per_key["color_"+ckey[0]+"-"+ckey[1]+"-"+ckey[2]];
        });
    });

    chart.selectAll(".seconds")
        .data(x.ticks(num_ticks))
        .enter().append("text")
            .attr("class", "labels seconds drawings")
            .attr("x", x)
            .attr("y", chart_height - 10)
            .attr("dy", -3)
            .attr("text-anchor", "middle")
            .attr("fill", "#fff")
            .text(function(value) {
                return d3.round(value / 1.0e9, 9) + " s";
            });

	plot_empty = false;
	
}

function static_plot() {
	
	if (typeof db_data == "undefined") { return; }
	
	key_list = Object.keys(db_data).sort();
	
	bar_height = y(1) - y(0);
	
	stats.selectAll(".stats").remove();
	
	stats.selectAll(".stats")
		.data(key_list)
		.enter().append("text")
			.attr("class", "stats labels")
			.attr("x", statistics_width - 10)
			.attr("y",  function(d, i) { return y(i) + bar_height / 2 - 10; })
			.attr("text-anchor", "end")
			.attr("alignment-baseline", "middle")
			.attr("fill", "#000")
			.text(function(key) {
			
				duration = db_data[key][db_data[key].length - 1][1] - db_data[key][0][0];
				duration_eff = 0.0;
				
				db_data[key].forEach(function (value, index, array) {
					duration_eff += value[1] - value[0];
				});
				
				return  db_data[key].length + "x, " +
						d3.round(duration / 1.0e9, 4) + "s (" +
						d3.round(duration_eff / 1.0e9, 4) +  "s)";
				
			});
	
}
