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
var stats;

var chart_width;
var chart_height;

var padding = 10;

var bottom_space = 30;

var rel_mouse_position = 0;

var num_experiments_to_load;

var prev_x_position = NaN;

var circle_dim;

var plot_empty = true;

var statistics_open = false;
var statistics_width = 200;

/*
 * Initial routines
 */
$(document).ready(function(){
    //$("#overlay").hide();

    hide_circle();
    
    $("#statistics_content").width(0);
    
    $("#db_entries").css('top', window.innerHeight / 2 - $("#db_entries").height() / 2);

    $("#db_entries").each(function (index, value) {
        $(this).click(function(event) { select_db($(event.target).html()); });
    });
        
    resize_data_content();
    
    select_db("kmeans.db"); //TODO: REMOVE THIS
    
    circle_dim = [$("#circle").width(), $("#circle").height()];
    
    $("#statistics_btn").click(function(event) {
        
        if (statistics_open) {
            
            $(this).animate({width: 28})
            $("#statistics_content").animate({width: 0})
            statistics_open = false;
            
        } else {
            
            $(this).animate({width: statistics_width + 28});
            $("#statistics_content").animate({width: statistics_width})
            statistics_open = true;
            
        }
    });
    
    stats = d3.select("#statistics_content").append("svg:svg")  
        .attr("class", "stats")
        .attr("id", "stats_svg")
        .attr("width", "100%") //chart_width)
        .attr("height", "100%"); //chart_height);
    
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

var call_text = function(d) {
    if (d > 1) {
        return " Calls";
    } else {
        return " Call";
    }
}

function create_color_pickers() {
    $('.color-picker').each(function(index, value) {
        $(this).minicolors({
            change: function(hex, rgba) {
                $($(this).parent().children()[2]).css('color', hex);
                colors_per_key[$(this).attr('id')] = hex;
                plot();
            }
        })

        $(value).parent().css('display', 'none');
        $(value).parent().css('float', 'left');
        $(value).parent().css('margin-right', '5px');
        $(value).parent().css('margin-top', '5px');
        $(value).parent().css('z-index', '99999999');
    });
}
