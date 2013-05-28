$(document).ready(function() {
    e = new elapsd();

    $("#overlay").show();

    db_selector = $("#db_selector");
    db_selector
        .css('top', $(window).innerHeight() / 2 - db_selector.outerHeight() / 2)
        .css('left', $(window).innerWidth() / 2 - db_selector.outerWidth() / 2);

    url_params = window.location.search.substring(1).split('&');
    $.each(url_params, function(index, value) {
        param = value.split('=')

        if (param[0] == '_debugmode' && param[1] == 'true') {
            e.changeDB("GLAT1.1.db", db_selector);
        }
    });

});

$(window).resize(function() {
    e.resizeDocument();
});

