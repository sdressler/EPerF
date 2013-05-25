var e;

function elapsd() {

    var experiments = $('#experiments');
    var drawing     = $('#drawing');
    var footer      = $('#footer');

    var min_width   = 2;
    var num_ticks   = 15;

	var chart = d3.select('#drawing').append("svg:svg")	
		.attr("class", "chart")
		.attr("id", "chart")
		.attr("width", "100%");
	//	.attr("height", "100%");
    var $_chart = $("#chart");

    var chart_seconds = d3.select('#drawing').append("svg:svg")
        .attr("class", "chart")
        .attr("id", "chart_seconds")
        .attr("width", "100%")
        .attr("height", "50");
    var $_chart_seconds = $("#chart_seconds");

    var plot_empty = true;

    this.resizeDocument = function() {
        drawing.outerHeight(
              $(window).innerHeight()
            - experiments.outerHeight()
            - footer.outerHeight()
            - 20
        );

        // Resize the charts SVG's
        $_chart.innerHeight(drawing.innerHeight() - $_chart_seconds.innerHeight());

        // Update scales and replot
        if (plot_empty == false) {
            e.updateScales();
            e.replot();
        }
    };

    this.changeDB = function(db) {
        this.db = db;

        // Trigger selection creation
        this.createExpSelection();
    };

    var x, y;
    var db_data = {};

    this.createExpSelection = function() {
        
        this.exp_selection = {}; // Re-Initialize experiments object

        var e             = this;
        var exp           = experiments;
        var exp_selection = this.exp_selection;

        var exp_to_load   = 0;
        var exp_loaded    = 0;

        $.ajax({
            type: 'GET',
            url: '/get_experiments',
            dataType: 'json',
            data: { db: this.db },
            async: true,
            success: function(data) {

                exp_to_load = data.result.length;

                $.each(data.result, function(index, value) {

                    var exp_id = value[0];

                    exp_selection[exp_id] = {
                        'exp_date': new Date(value[1] * 1000),
                        'exp_name': value[2],
                        'exp_start': value[3],
                        'exp_stop': value[4],
                        'exp_data': {}
                    };

                    // Call to get details of experiments
                    $.ajax({
                        type: 'GET',
                        url: '/get_experiment_overview',
                        dataType: 'json',
                        data: { id: exp_id },
                        async: true,
                        success: function(data) {
                            $.each(data.result, function(index, value) {
                                exp_selection[exp_id]
                                    .exp_data[value[0] + '-' + value[2]] = {
                                        'kname': value[1],
                                        'dname': value[3],
                                        'selected': false
                                    }
                            });
                            
                            exp_loaded++;

                            if (exp_loaded == exp_to_load) {
                                redrawExperiments(exp, exp_selection);
                                e.resizeDocument();
                            }

                        }
                    });
                });
            }
        });
    };

    function redrawExperiments(exp, exp_selection) {

        // Cleanup
        $('.experiment.container').remove();

        // Draw
        $.each(exp_selection, function(key,value) {
            $('<div>', {
                'class': 'experiment container',
                'id': 'exp-' + key
            }).appendTo(exp);

            $('<div>', {
                'class': 'experiment title',
                'text': exp_selection[key].exp_date
            }).appendTo("#exp-" + key);

            $('<div>', {
                'class': 'experiment title',
                'text': exp_selection[key].exp_name
            }).appendTo("#exp-" + key);

            $.each(exp_selection[key].exp_data, function(subkey,value) {

                var kid = subkey.split('-')[0];
                var did = subkey.split('-')[1];

                $('<div>', {
                    'class': 'experiment selectable',
                    'text': value.kname + " - " + value.dname,
                    'eid': key,
                    'kid': kid,
                    'did': did,
                    'onclick': 'e.triggerSelect(this)'
                }).appendTo("#exp-" + key);
            })

        });
    };

    this.triggerSelect = function(caller) {

        var caller = $(caller);

        var eid = caller.attr('eid');
        var sid = caller.attr('kid') + '-' + caller.attr('did');

        if (caller.hasClass('selected')) {
            // Already selected, undo    
            caller.removeClass('selected');
            e.exp_selection[eid].exp_data[sid].selected = false;
        } else {
            // Select it
            caller.addClass('selected');
            e.exp_selection[eid].exp_data[sid].selected = true;
        }

        this.redrawFromSelection();

    };

    this.redrawFromSelection = function() {
  
        /* Create the selection */
        var selection = [];
        $.each(this.exp_selection, function(ekey,evalue) {
            $.each(evalue.exp_data, function(dkey,dvalue) {
                if (dvalue.selected) {
                    selection.push([
                        ekey, dkey.split('-')[0], dkey.split('-')[1]
                    ]);
                }
            });
        });

        /* Store the current selection */
        this.selection = selection;

        /* Maybe nothing was selected ?! */
        if (selection.length == 0) {
            this.clearPlot();
            return;
        }

        var e = this;

        /* Trigger the AJAX request to receive the data */
        $.ajax({
            type: 'POST',
            url: '/get_data',
            dataType: 'json',
            data: { sel: selection },
            async: true,
            success: function(data) {

                /* Store data */
                db_data = data.result;

                /* Get maximum */
                var max = [];
                
                $.each(db_data, function(key,value) {
                    max.push(d3.max(value.map(function(x) { return x[1]; })));
                });
         
                e.max_value = d3.max(max);

                e.updateScales(true);
                e.replot();

            }
        });
    };

    this.updateScales = function(both) {
    
	    // Only if data is available
    	if (typeof db_data == "undefined") { return; }

        if (both) {
            x = d3.scale.linear()
                .domain([0, e.max_value])
                .rangeRound([10, $_chart.innerWidth() - 20]);
        }

    	y = d3.scale.linear()
	    	.domain([0, Object.keys(db_data).length])
            .rangeRound([0, $_chart.innerHeight()]);
        
        d3.select("#chart").call(d3.behavior.zoom()
            .x(x)
            .on("zoom", function() {
                e.replot();
            })
        );

    }

    this.clearPlot = function() {
        // Cleanup everything
        d3.selectAll(".drawings").remove();
    }

    this.replot = function() {

        // Only if data is available
        if (typeof db_data == "undefined") { return; }
    
        var draw_data = {};
        var key_index = {};
        var idx = 0;

        // Preselect to match current plotting range
        $.each(db_data, function(key, obj) {

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

        e.clearPlot();

        // Draw the grid
        chart.selectAll("lines")
            .data(x.ticks(num_ticks))
            .enter().append("line")
                .attr("class", "drawings")
                .attr("x1", function(d) { return x(d) + 0.5; })
                .attr("x2", function(d) { return x(d) + 0.5; })
                .attr("y1", 0)
                .attr("y2", $_chart.innerHeight())
                .attr("id", "vgrid")
                .style("stroke", "#bbb")
                .style("stroke-width", "1.5px")
                .style("stroke-dasharray", "5,5");

        chart_seconds.selectAll(".seconds")
            .data(x.ticks(num_ticks))
            .enter().append("text")
                .attr("class", "labels seconds drawings")
                .attr("x", x)
                .attr("y", $_chart_seconds.height() / 2)
                .attr("dy", -3)
                .attr("text-anchor", "middle")
                .attr("fill", "#fff")
                .text(function(value) {
                    return d3.round(value / 1.0e9, 9) + " s";
                });
        
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
/*
            chart.selectAll(".rect-" + key).attr("fill", function() {
                ckey = key.split("-");
                return colors_per_key["color_"+ckey[0]+"-"+ckey[1]+"-"+ckey[2]];
            });
*/            
        });
        
        plot_empty = false;

    }

    var bisect_l = d3.bisector(function(d) { return d[0]; }).left;
    var bisect_r = d3.bisector(function(d) { return d[1]; }).right;

};
