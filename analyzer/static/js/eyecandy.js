function show_overlay() {
	$("#overlay").fadeIn('fast');

	$("#overlay_content").offset({
		top: window.innerHeight / 2 - $("#overlay_content").height() / 2,
		left: window.innerWidth / 2 - $("#overlay_content").width() / 2
	});
	
}

function hide_overlay() { $("#overlay").fadeOut('fast'); }