function elapsd() {

    var e = this;

    var experiments = $('#experiments');
    var drawing     = $('#drawing');
    var footer      = $('#footer');
    var overlay     = $('#overlay');

    var bisect_l = d3.bisector(function(d) { return d[0]; }).left;
    var bisect_r = d3.bisector(function(d) { return d[1]; }).right;

    var spinner = new Spinner({
        color:'#00B1E6',
        lines: 12,
        length: 0,
        width: 8,
        radius: 14
    });

    overlayToggle = function(newState) {
            
        if (newState == 'show') {
            overlay.show();
            spinner.spin(document.getElementById("overlay"));
        } else {
            overlay.fadeOut();
            spinner.stop();
        }

    }

    this.setThreadInterleave = function(interleave) {
        this._threadInterleave = interleave;
        this.replot();
    }

    var min_width   = 2;
    var num_ticks   = 15;

    var chart_top_space = 25;

    var current_precision = 3;

    var active_markers = {};
    var cursor_sec;

	var chart = d3.select('#drawing').append("svg:svg")	
		.attr("class", "chart")
		.attr("id", "chart")
		.attr("width", "100%")
    var $_chart = $("#chart");

        $_chart
            .mousemove(function(ev) {
                
                d3.selectAll('.vline').remove();

                if (!ev.ctrlKey || x == 'undefined') {
                    $(this).css('cursor', 'pointer');
                    return;
                }
                
                $(this).css('cursor', 'none');

                var pos = ev.pageX
                        - parseInt(drawing.css('margin-left'))
                        - parseInt(drawing.css('padding-left'))
    
                cursor_sec = x.invert(pos) / 1.0e9;

                var precision;
                if (current_precision < 3 || isNaN(current_precision)) {
                    precision = 3;
                } else {
                    precision = current_precision;
                }

                d3.select("#drawing").append('div')
                    .attr('class', 'chart vline tag')
                    .attr('id', 'vline_tag')
                    .style('left', ev.pageX)
                    .style('top', $_chart.position().top - 5)
                    .text(cursor_sec.toPrecision(precision) + "s")

                d3.select('#chart').append('line')
                    .attr('class', 'vline')
                    .attr('id', 'vline')
                    .attr('x1', pos + 0.5)
                    .attr('x2', pos + 0.5)
                    .attr('y1', 0)
                    .attr('y2', $_chart.innerHeight())
                    .style("stroke", "#fff")
                    .style("stroke-width", "1.5px");

            })
            .click(function(ev) {

                if (ev.ctrlKey) {
                    active_markers[cursor_sec] = true;
                    e.drawMarkers();
                }

            });

    var chart_seconds = d3.select('#drawing').append("svg:svg")
        .attr("class", "chart")
        .attr("id", "chart_seconds")
        .attr("width", "100%")
        .attr("height", "50");
    
    var $_chart_seconds = $("#chart_seconds");

    var plot_empty = true;

    this.colors = [
        '#6895E7',
        '#B468E7',
        '#7FF26D',
        '#FFCE73',

        '#27509B',
        '#6C279B',
        '#3DAC2B',
        '#BF8D30',

        '#002F86',
        '#500087',
        '#149500',
        '#A66C00',

        '#3A77E7',
        '#A13AE7',
        '#55F23D',
        '#FFBC40',

        '#0049CE',
        '#7B00CF',
        '#1FE600',
        '#FFA500'
    ];

    this.resizeDocument = function() {
        drawing.outerHeight(
              $(window).innerHeight()
            - experiments.outerHeight()
            - footer.outerHeight()
            - 20
        );

        // Resize the charts SVG's
        $_chart.innerHeight(
              drawing.innerHeight()
            - $_chart_seconds.innerHeight()
            - chart_top_space
        );

        $_chart.css('margin-top', chart_top_space);

        // Update scales and replot
        if (plot_empty == false) {
            this.updateScales('both');
            this.replot();
        }
    };

    this.changeDB = function(db, sender) {

        $(sender).hide();
        overlayToggle('show');

        this.db = db;

        // Trigger selection creation
        this.createExpSelection();
    };

    var x, y;
    var db_data = {};

    this.createExpSelection = function() {
        
        this.exp_selection = {}; // Re-Initialize experiments object

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

                                overlayToggle('hide');

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
                    'did': did
                }).appendTo("#exp-" + key)
                  .click(function(sender) { e.triggerSelect(sender.target); });
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

            this.exp_selection[eid].exp_data[sid].selected = false;
            this.colors.push(this.exp_selection[eid].exp_data[sid].color);

        } else {

            overlayToggle('show');

            // Select it
            caller.addClass('selected');
            this.exp_selection[eid].exp_data[sid].selected = true;

            var color = this.colors.pop();

            this.exp_selection[eid].exp_data[sid].color = color;
            caller.css('border-left-color', color);

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
                        ekey,
                        dkey.split('-')[0],
                        dkey.split('-')[1],
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

                var max_eid = 0;
                var max_kid = 0;
                var max_did = 0;
                var max_tid = 0;
                
                $.each(db_data, function(key,value) {

                    subkeys = key.split("-");

                    max_eid = d3.max([max_eid, parseInt(subkeys[0])]);
                    max_kid = d3.max([max_kid, parseInt(subkeys[1])]);
                    max_did = d3.max([max_did, parseInt(subkeys[2])]);
                    max_tid = d3.max([max_tid, parseInt(subkeys[3])]);

                    max.push(d3.max(value.map(function(x) {
                        return x[1];
                    })));
                });
      
                var precisions = [
                    getPrecision(max_eid),
                    getPrecision(max_kid),
                    getPrecision(max_did),
                    getPrecision(max_tid)
                ];

                $.each(db_data, function(key, value) {
                    
                    var subkeys = key.split("-");
                    var new_key = "";

                    $.each(subkeys, function(index, skey) {
                        new_key += getPaddedNumString(skey, precisions[index]) + "-";
                    });
                    new_key = new_key.slice(0,-1);

                    // Exchange keys
                    if (new_key != key) {
                        db_data[new_key] = db_data[key];
                        delete db_data[key];
                    }

                });
                
                e.max_value = d3.max(max);

                e.updateScales('both');
                e.replot();

                overlayToggle('hide');

            }
        });
    };

    getPrecision = function(number) {
        if (number == 0) { number = 1; }
        return parseInt(Math.log(number) / Math.LN10 + 1);
    }

    getPaddedNumString = function(strnum,digits) {
        var pad = digits - strnum.length;
        return Array(pad + 1).join("0") + strnum;
    }

    this.updateScales = function(scale) {
    
	    // Only if data is available
    	if (typeof db_data == "undefined") { return; }

        if (scale == 'x' || scale == 'both') {
            x = d3.scale.linear()
                .domain([0, e.max_value])
                .rangeRound([10, $_chart.innerWidth() - 20]);
        }

        if (scale == 'y' || scale == 'both') {

            var domain_max;
            if (this._threadInterleave == 'line') {
                domain_max = this._threads_per_group; 
            } else {
                domain_max = Object.keys(db_data).length;
            }
            
            y = d3.scale.linear()
                .domain([0, domain_max])
                .rangeRound([0, $_chart.innerHeight()]);
        }
        
        d3.select("#chart").call(d3.behavior.zoom()
            .x(x)
            .on("zoom", function() {
                var translate = d3.event.translate;

                current_precision = parseInt(Math.log(
                    (x.domain()[0] + translate[0]) / 1.0e9 * d3.event.scale
                ) / Math.LN10 + 1);
                
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
        if ($.isEmptyObject(db_data)) { return; }   

        var draw_data = {};
        var keys = [];
   
        e._threads_per_group = 1;

        // Preselect to match current plotting range
        $.each(db_data, function(key, obj) {

            var draw_data_entry = {};

            lo = bisect_l(db_data[key], x.domain()[0]) - 1;
            hi = bisect_r(db_data[key], x.domain()[1]) + 1;

            if (lo == -1) { lo = 0; }
            if (hi > (db_data[key].length - 1)) { hi = db_data[key].length - 1; }

            /* This merges elements that are to close to each other */
            draw_data_entry.data = []
            draw_data_entry.data.push([x(db_data[key][lo][0]),x(db_data[key][lo][1])]);
            var start, stop;
            var j = 0;
            for (var i = lo + 1; i < hi; i++) {

                start = x(db_data[key][i][0]);
                stop  = x(db_data[key][i][1]);

                if ((start - draw_data_entry.data[j][1]) < min_width) {
                    draw_data_entry.data[j][1] = stop;
                } else {
                    draw_data_entry.data.push([start,stop]);
                    j++;
                }
            }

            subkeys = key.split('-');
            draw_data_entry.color = e.exp_selection[
                                        subkeys[0]
                                    ].exp_data[subkeys[1] + '-' + subkeys[2]].color;

            draw_data_entry.key = key;
            draw_data_entry.tid = parseInt(subkeys[3]);

            group_key = ""
            if (e._threadInterleave == "true" ||
                e._threadInterleave == "line")
            {
                group_key = subkeys[3];
                e._threads_per_group = d3.max([
                    e._threads_per_group,parseInt(subkeys[3])
                ]);
            }/* else {
                group_key = subkeys[0] + subkeys[1] + subkeys[2];
                group_key = "";
            }*/
            
            prefix_key = group_key + key;

            keys.push(prefix_key);
            draw_data[prefix_key] = draw_data_entry;
        });

        this.updateScales('y');
        this.clearPlot();

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
   
        $.each(keys.sort(d3.ascending), function(index, key) {

            value = draw_data[key];

            chart.selectAll(".rect-" + value.key)
                .data(value.data)
                .enter().append("rect")
                    .attr("class", "drawings rect-" + value.key)
                    .attr("y", function(d) {
                    
                        var _y = 0;
                        if (e._threadInterleave != 'line') {
                            _y = y(index);
                        } else {
                            _y = y(value.tid);
                        }

                        return _y;

                    })
                    .attr("x", function(d) { return d[0]; })
                    .attr("width", function(d) {
                        w = d[1] - d[0];
                        
                        if (w < min_width) { return min_width; }

                        return w;
                    })
                    .attr("height", bar_height - 5)
                    .attr("fill", value.color);
        });

        this.drawMarkers();

        plot_empty = false;

    }

    this.drawMarkers = function() {

        var precision;
        if (current_precision < 3 || isNaN(current_precision)) {
            precision = 3;
        } else {
            precision = current_precision;
        }

        d3.selectAll(".marker").remove();

        $.each(active_markers, function(key,value) {

            var pos = x(key * 1.0e9);

            if (pos < 0) { return; }

            d3.select("#drawing").append('div')
                .attr('class', 'chart marker tag removeable')
                .attr('id', 'vline_tag')
                .attr('key', key)
                .style('left', pos + 19)
                .style('top', $_chart.position().top - 5)
                .text(parseFloat(key).toPrecision(precision) + "s")
                .on('click', function(ev) {
                    delete active_markers[key];
                    $("div[key='" + key + "']").remove();
                    $("line[key='" + key + "']").remove();
                })
            
            d3.select('#chart').append('line')
                .attr('class', 'marker chart')
                .attr('key', key)
                .attr('x1', pos + 0.5)
                .attr('x2', pos + 0.5)
                .attr('y1', 0)
                .attr('y2', $_chart.innerHeight())
                .style("stroke", "#fff")
                .style("stroke-width", "3px")
                .style("stroke-dasharray", "5,5");

        });
    }

};
