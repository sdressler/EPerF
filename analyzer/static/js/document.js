$(document).ready(function() {
    e = new elapsd();
    e.changeDB("kmeans.db");
});

$(window).resize(function() {
    e.resizeDocument();
});

