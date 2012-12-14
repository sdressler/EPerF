function show_overlay() {
	$("#overlay").fadeIn('fast');

	$("#overlay_content").offset({
		top: window.innerHeight / 2 - $("#overlay_content").height() / 2,
		left: window.innerWidth / 2 - $("#overlay_content").width() / 2
	});
	
}

function hide_overlay() { $("#overlay").hide(); }

function hide_footer(duration) {	
	$(".footer.section.info").fadeOut(duration || 'slow');
	    //.css('background-color', '#222')
	    //.css('color', '#222');
}

function show_footer(duration) {
	$(".footer.section.info").fadeIn(duration || 'slow');
		//.css('background-color', '#00B1E6')
		//.css('color', '#000');
}