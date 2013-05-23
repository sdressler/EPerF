function fetch_data_from_db_for_selections() {

    /* Create the selection */
	var selection = [];
	$(".selected").each(function(idx, elem) {
		var experiment = $(elem).parent().parent().attr('id');
		var kernel = $(elem).attr('kid');
		var device = $(elem).attr('did');
		
		selection.push([experiment, kernel, device]);
	});
    /* end */
	
    /* Test whether *nothing* is selected */
	if (selection.length == 0) { clear_plot(); return; }

    /* Trigger overlay to notify user */    
	show_overlay();
	
	/* Set up AJAX request and trigger it */
    $.ajax({
	    type: 'POST',
	    url: '/get_data',
	    dataType: 'json',
	    data: { sel: selection },
	    async: true,
	    success: function(data) {

	    	db_data = data.result;
	    	max = [];
	    	
	    	// Get the maximum value
	    	Object.keys(db_data).forEach(function(key, index, array) {
	    		max.push(d3.max(
                    db_data[key].map(function(value, index) {
                        return value[1];
                    })
                ))
	    	});
	  
            max_x = d3.max(max);
//	    	max_x = (2.0 * d3.max(max)) * 0.01;
	    
			update_scales();

			static_plot();
			plot();
			
			hide_overlay();
	    	
	    }
	});
    /* end */
	
}
