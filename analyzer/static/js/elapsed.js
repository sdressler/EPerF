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

    overlayToggle = function(newState, msg) {

        $("#overlay_msg").text(msg);

        // Only update message if state is the same
        if (newState == e._current_overlay_state || newState == null) { return; }    

        if (newState == 'show') {
            overlay.show();
            $("#overlay_msg").show();

            spinner.spin(document.getElementById("overlay"));
            e._current_overlay_state = 'show';
        } else {
            overlay.fadeOut();
            spinner.stop();
            e._current_overlay_state = 'hide';
        }


    }

    this.setThreadInterleave = function(interleave) {
        this._threadInterleave = interleave;
        this.changeDisplay();
    }

    var min_width   = 2;
    var min_height  = 2;
    var num_ticks   = 15;

    var chart_top_space = 25;

    this._precision = 3;
    this.precision = function(value) {
        if (value == null) {
            return this._precision;
        } else {
            if (value < 3) {
                this._precision = 3;
            } else if (value > 9) {
                this._precision = 9;
            } else {
                this._precision = value;
            }
        }
    }

    var active_markers = {};
    var cursor_sec;

    var current_selection = {};

	var chart = d3.select('#drawing').append("svg:svg")	
		.attr("class", "chart")
		.attr("id", "chart")
		.attr("width", "100%")
    var $_chart = $("#chart");

        $_chart
            .mousemove(function(ev) {
                
                d3.selectAll('.vline').remove();

                if (!ev.ctrlKey || x == 'undefined' || x == null) {
                    $(this).css('cursor', 'crosshair');
                    $_tag.hide();
                    return;
                }
                
                $(this).css('cursor', 'none');

                var pos = ev.pageX
                        - parseInt(drawing.css('margin-left'))
                        - parseInt(drawing.css('padding-left'))
    
                cursor_sec = x.invert(pos) / 1.0e9;

                $_tag
                    .show()
                    .css('left', ev.pageX)
                    .css('top', $_chart.position().top - 5)
                    .text(e.numberToString(cursor_sec) + "s");

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

    d3.select("#drawing").append('div')
        .attr('class', 'chart tag')
        .attr('id', 'tag')
//        .style('left', ev.pageX)
//        .style('top', $_chart.position().top - 5)
//        .text(cursor_sec.toPrecision(precision) + "s")

    $_tag = $("#tag");

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

        // Replace overlay message
        $("#overlay_msg")
            .css('top', $(window).innerHeight() / 2 + 100)
            .hide();

        // Update scales and replot
        if (plot_empty == false) {
            this.updateScales('both');
            this.replot();
        }
    };

    this.changeDB = function(db, sender) {

        $(sender).hide();

        overlayToggle('show', 'Switching to ' + db);

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

            xkey = eid + '-' + sid;
            $.each(db_data, function(key,value) {
                if (key.search(xkey) != -1) {
                    delete db_data[key];
                }
            })

        } else {

            overlayToggle('show', 'Loading selected data.');

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
        var new_selection = {};
        $.each(this.exp_selection, function(ekey,evalue) {
            $.each(evalue.exp_data, function(dkey,dvalue) {
                if (dvalue.selected) {
                    new_selection[ekey + "-" + dkey] = true;
                }
            });
        });

        var load_selection = [];
        $.each(new_selection, function(key,value) {

            if (current_selection[key] == null) {
                sel_key = key.split('-');
                load_selection.push([
                    parseInt(sel_key[0]),
                    parseInt(sel_key[1]),
                    parseInt(sel_key[2])    
                ])
            }

        });

        /* Store the current selection */
        current_selection = new_selection;

        /* Maybe nothing was selected ?! */
        if (Object.keys(current_selection).length == 0) {
            this.clearPlot();
            return;
        }

        /* Nothing to reload, but we should trigger a redraw */
        if (load_selection.length == 0) {
            this.changeDisplay();
            return;
        }

        /* Trigger the AJAX request to receive the data */
        overlayToggle(null, 'Data request pending.');
        $.ajax({
            type: 'POST',
            url: '/get_data',
            dataType: 'json',
            data: { sel: load_selection },
            async: true,
            success: function(data) {

                var prefix_key = Object.keys(data.result)[0].split('-');
                    prefix_key = prefix_key[0] + "-" + prefix_key[1] + "-" + prefix_key[2];

                /* Make Thread IDs consecutive in any case */
                var tids = []
                $.each(data.result, function(key,value) {
                    tids.push(parseInt(key.split('-')[3]));
                });

                var suffixes = {}
                $.each(tids.sort(d3.ascending), function(idx,val) {
                    suffixes[val] = idx;
                });

                /* Store and rewrite TID if necessary */
                $.each(data.result, function(key,value) {
                    var suffix_key = key.split('-')[3];

                    if (suffix_key != suffixes[suffix_key]) {
                        new_key = prefix_key + "-" + suffixes[suffix_key];
                        db_data[new_key] = value;
                    } else {
                        db_data[key] = value;
                    }
                });

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
                e.changeDisplay();

            }
        });
    };

    this.changeDisplay = function() {

        overlayToggle('show', 'Changing View.');

        // Prepare Draw Data
        e._full_draw_data = e.prepareDrawData(db_data);

        // Update scales
        e.updateScales('both');
     
        if (this._threadInterleave != 'line') { 
            this._bar_height = y.range()[1] / this._total_num_threads;
        } else {
            var max_thread_num = 0;
            $.each(this._thread_groups, function(key,value) {
                max_thread_num = Math.max(max_thread_num, value.threads);
            })
            this._bar_height = y.range()[1] / max_thread_num;
        }

        if ((this._bar_height - this._bar_space) < min_height) {
            this._bar_height = min_height + this._bar_space;
        }

        e.replot();
        
        overlayToggle('hide');
    
    }

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

            var domain_max = 1;
            if (this._thread_groups != null && this._threadInterleave != 'line') {
                domain_max = Object.keys(this._thread_groups).length;
            } else {
                domain_max = 1;
            }

            y = d3.scale.linear()
                .domain([0, domain_max])
                .rangeRound([0, $_chart.innerHeight()]);
        }
        
        d3.select("#chart").call(d3.behavior.zoom()
            .x(x)
            .on("zoom", function() {
                var translate = d3.event.translate;

                var low = (x.domain()[0] + d3.event.translate[0]) / 1.0e9;
                var hi  = (x.domain()[1] + d3.event.translate[1]) / 1.0e9;

                e.precision(Math.round(1 / Math.sqrt(hi - low)));
                
                e.replot();
            })
        );

    }

    this.clearPlot = function() {
        // Cleanup everything
        d3.selectAll(".drawings").remove();
    }

    this.prepareDrawData = function(data) {

        var prepared_draw_data = [];

        var sorted_keys = Object.keys(data).sort(d3.ascending);

        this._thread_groups = {};
        var group_id = 0;
       
        var total_threads = 0;

        $.each(sorted_keys, function(idx,key) {
            var y_idx  = 0;

            var subkeys = key.split('-');
            var group_key = subkeys[0] + subkeys[1] + subkeys[2];

            if (e._thread_groups[group_key] == null) {
                e._thread_groups[group_key] = {
                    "group_id": group_id,
                    "threads": 0,
                    "effective_threads": 0,
                    "y_ends": []
                }
                group_id++;
            }

            var thread_group = e._thread_groups[group_key];
                thread_group.threads++;

            if (e._thread_limit) {
            
                var found = false;

                for (i = 0; i < thread_group.y_ends.length; i++) {
                    if (data[key][0][0] > thread_group.y_ends[i]) {
                        y_idx = i;
                        found = true;
                        break;
                    }
                }

                if (!found) {
                    thread_group.y_ends.push(0);
                    y_idx = thread_group.y_ends.length - 1;
                }

                thread_group.y_ends[y_idx] = data[key][data[key].length - 1][1];
                thread_group.threads = thread_group.y_ends.length;

            } else {
                y_idx = subkeys[3]; 
            }

            prepared_draw_data.push({
                'data': data[key],
                'y_idx': parseInt(y_idx),
                'group_key': group_key,
                'color': e.exp_selection[subkeys[0]].exp_data[subkeys[1] + '-' + subkeys[2]].color
            });
            
        });

        this._total_num_threads = 0;
        $.each(this._thread_groups, function(key,value) {
            e._total_num_threads += value.threads;;
            value.num_threads = value.threads;
        });

        /* This adjusts the y-position */
        var groups = Object.keys(this._thread_groups).length;
        $.each(prepared_draw_data, function(key,value) {
            var gid = e._thread_groups[value.group_key].group_id;
            var num_threads = e._thread_groups[value.group_key].num_threads;

            if (e._threadInterleave == 'true') {
                value.y_idx = (value.y_idx / num_threads) * groups + (gid / num_threads);
            } else if (e._threadInterleave == 'line') {
                value.y_idx = (value.y_idx / num_threads); // Line Interleave
            } else {
                value.y_idx = value.y_idx / num_threads + gid; // Original
            }

        });
            
        return prepared_draw_data;

    }
       
    this._bar_space = 5;

    this.replot = function() {

        // Only if data is available
        if ($.isEmptyObject(this._full_draw_data)) { return; }   

        var draw_data = [];
        $.each(this._full_draw_data, function(key,value) {

            var obj = e._full_draw_data[key];
            var data = obj.data;

            var lo = bisect_l(data, x.domain()[0]) - 1;
            var hi = bisect_r(data, x.domain()[1]) + 1;

            if (lo == -1) { lo = 0; }
            if (hi > (data.length - 1)) { hi = data.length - 1; }

            /* This merges elements that are to close to each other */
            data_array = [[x(data[lo][0]),x(data[lo][1])]];
            var start, stop;
            var j = 0;
            for (var i = lo + 1; i < hi; i++) {

                start = x(data[i][0]);
                stop  = x(data[i][1]);

                if ((start - data_array[j][1]) < min_width) {
                    data_array[j][1] = stop;
                } else {
                    data_array.push([start,stop]);
                    j++;
                }
            }
            
            draw_data.push({
                'data': data_array,
                'y_idx': obj.y_idx,
                'group_key': obj.group_key,
                'color': obj.color
            });
        });
        
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

        $.each(draw_data, function(idx,value) {
            chart.selectAll("drawings")
                .data(value.data)
                .enter().append("rect")
                .attr("class", "drawings")
                .attr("y", y(value.y_idx))
                .attr("x", function(d) { return d[0]; })
                .attr("width", function(d) {
                    w = d[1] - d[0];
                     
                    if (w < min_width) { return min_width; }
    
                    return w;
                })
                .attr("height", e._bar_height - e._bar_space)
                .attr("fill", value.color);
                
        });

/*
 *                    .on("mousemove", function() {
 *
 *                        var rect_key = d3.event.target.id;
 *                        var key = rect_key.replace("rect-", "");
 *
 *                        var x_pos = $(d3.event.target).position().left;
 *                        var y_pos = parseInt($(d3.event.target).attr("width")) + x_pos;
 *
 *                        $_tag
 *                            .show()
 *                            .css('left', d3.event.pageX)
 *                            .css('top', d3.event.pageY + $_tag.outerHeight())
 *                            .text(
 *                                (x.invert(x_pos - $_chart.position().left) / 1.0e9).toPrecision(e.precision()) + "s " +
 *                                (x.invert(y_pos - $_chart.position().left) / 1.0e9).toPrecision(e.precision()) + "s " +
 *                                db_data[key].length
 *                            );
 *
 *                    })
 *                    .on("mouseout", function() {
 *                        $_tag.hide();
 *                    });
 */

        this.drawMarkers();
        plot_empty = false;

    }

    this.numberToString = function(value) {
        return String(value.toFixed(e.precision()));
    }

    this.drawMarkers = function() {

        var precision = this.precision();
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
                .text(parseFloat(key).toFixed(precision) + "s")
                .on('click', function(ev) {
                    delete active_markers[key];
                    $("div[key='" + key + "']").remove();
                    $("line[key='" + key + "']").remove();
                });
            
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

    $("#threads_max").change(function(ev) {
        e._thread_limit = $(this).is(':checked');
        e.changeDisplay();
    });

};
