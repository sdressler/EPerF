function show_overlay() {
	$("#overlay").fadeIn('fast');

	$("#overlay_content").css(
	    'top', window.innerHeight / 2 - $("#overlay_content").height() / 2
	);
	
}

function hide_overlay() { $("#overlay").hide(); }