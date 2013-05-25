
/*
 * 
 * This function is necessary in order to make the plotting SVG bigger without
 * oversizing the window
 * 
 */
function resize_data_content() {
	
	$("#chart_container")
		.width(window.innerWidth - $("#statistics").width() - 15)
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
	chart_height = $("#chart_container").height() - 20;
	
	d3.select("#chart_svg")
		.remove();
	
	chart = d3.select("#chart_container").append("svg:svg")	
		.attr("class", "chart")
		.attr("id", "chart_svg")
		.attr("width", "100%") //chart_width)
		.attr("height", "100%"); //chart_height);
	
	static_plot();

    /*
              translatePos=d3.event.translate;
              var value = zoomWidgetObj.value.target[1]*2;
     
              //detect the mousewheel event, then subtract/add a constant to the zoom level and transform it
              if (d3.event.sourceEvent.type=='mousewheel' || d3.event.sourceEvent.type=='DOMMouseScroll'){
                    if (d3.event.sourceEvent.wheelDelta){
                        if (d3.event.sourceEvent.wheelDelta &gt; 0){
                            value = value + 0.1;
                        }else{
                            value = value - 0.1;
                        }
                    }else{
                        if (d3.event.sourceEvent.detail &gt; 0){
                            value = value + 0.1;
                        }else{
                            value = value - 0.1;
                        }
                    }
              }
            transformVis(d3.event.translate,value);
    */
               
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
	
	plot_empty = true;
	
}

function update_scales() {
	
	// Only if data is available
	if (typeof db_data == "undefined") { return; }

/*    
	x = d3.scale.linear()
		.domain([0, max_x])
		.rangeRound([10, chart_width - 50]);
*/

    x = d3.time.scale()
            .domain([0, max_x])
            .rangeRound([10, chart_width - 50]);

	y = d3.scale.linear()
		.domain([0, Object.keys(db_data).length])
	    .rangeRound([bottom_space - 20, chart_height - bottom_space - 20]);
    
    d3.select("#chart_svg")
        .call(d3.behavior.zoom()
            .x(x)
            .on("zoom", function() {
                plot();
            }))
	
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

        lo = bisect_l(db_data[key], x.domain()[0]) - 1;
        hi = bisect_r(db_data[key], x.domain()[1]) + 1;

        if (lo == -1) { lo = 0; }
        if (hi > (db_data[key].length - 1)) { hi = db_data[key].length - 1; }

        //console.log([lo,hi]);

        /* This merges elements that are to close to each other */
        draw_data[key] = [];

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
			.attr("x1", function(d) { return x(d) + 0.5; })
			.attr("x2", function(d) { return x(d) + 0.5; })
			.attr("y1", 0)
			.attr("y2", chart_height - bottom_space)
			.attr("id", "vgrid")
			.style("stroke", "#bbb")
			.style("stroke-width", "1.5px")
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
