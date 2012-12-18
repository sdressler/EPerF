function show_overlay() {
	$("#overlay").fadeIn('fast');

	$("#overlay_content").css(
	    'top', window.innerHeight / 2 - $("#overlay_content").height() / 2
	);
	
}

function hide_overlay() { $("#overlay").hide(); }

function show_circle(msg) {
	
	if ($("#show_hints").prop("checked")) {
		$("#circle_text").text(msg);
		$("#circle").fadeIn();
	}
	
}

function hide_circle() { $("#circle").hide(); }

function circle_follow(event) {
	$("#circle")
		.css('left', event.pageX - circle_dim[0] + 215)
		.css('top', event.pageY - circle_dim[1] + 35);
}