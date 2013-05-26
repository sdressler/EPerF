$(document).ready(function() {
    e = new elapsd();

    $("#overlay").show();

    db_selector = $("#db_selector");
    db_selector
        .css('top', $(window).innerHeight() / 2 - db_selector.outerHeight() / 2)
        .css('left', $(window).innerWidth() / 2 - db_selector.outerWidth() / 2);

//    e.changeDB("kmeans.db");
});

$(window).resize(function() {
    e.resizeDocument();
});

