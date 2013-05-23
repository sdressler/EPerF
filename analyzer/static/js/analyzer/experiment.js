function load_experiments(db) {
	
	var e = $("#experiments");
	
	show_overlay();
	
	var experiments = [];
	
	$.ajax({
	    type: 'GET',
	    url: '/get_experiments',
	    dataType: 'json',
	    data: { db: db },
	    async: true,
	    success: function(data) {
	    	
	    	num_experiments_to_load = data.result.length - 1;
	    	
	    	data.result.forEach(function(value) {
	    		
	    		experiments.push(value[0]);
	    		
	    		var date = new Date(value[1] * 1000);
	    		
	    		e.append('\
	    		    <div class="container experiment" id="' + value[0] + '">\
	    		        <div class="entry experiment title">'
	    		    		+ '<div style="font-weight: bold">' + value[2] + '</div>'
	    		    		+ '<div>' + date.toString() + '</div> \
	    		        </div> \
	    		    	<div class="clear"></div> \
	    		    </div>');
	    		
	    		load_experiment_detail(value[0]);
	    		
	    	});
	    	
	    }
	});
		
}

function load_experiment_detail(experiment) {

	$.ajax({
	    type: 'GET',
	    url: '/get_experiment_overview',
	    dataType: 'json',
	    data: { id: experiment },
	    async: true,
	    success: function(data) {
	    	
	    	var ex = $("#" + experiment);
	    	
	    	data.result.forEach(function(entry) {
	    		ex.append(
	    			'<div>' +
	    				'<input class="color-picker" type="hidden" value="#B0BC00" id="color_' + experiment + '-' + entry[0] + '-' + entry[2] + '">' +
	    				'<div class="entry selectable" kid="' + entry[0] + '" did="' + entry[2] + '">'
	    					+ entry[1] + ' - ' + entry[3] +
	    				'</div>' +
	    				'<div class="clear"></div>' +
	    			'</div>'
	    		);
    		});
    		
    		if (num_experiments_to_load == 0) {
    			load_experiments_finalize();
    		} else {
    			num_experiments_to_load--;
    		}
    		
	    }
	    
	});
	
}
	
function load_experiments_finalize() {
	
	resize_data_content();
	create_color_pickers();
	
	// Make them all selectable
	$(".selectable").click(function(event) {

        var color_input = $(this).parent().find('input:first')[0]    
		var color = color_input.value;
	
		colors_per_key[color_input.id] = color; 
		
		if ($(this).hasClass("selected")) {
			
			// Already selected
			$(this).removeClass("selected");
			$(this).css('color', '#fff');

            $(color_input).parent().css('display', 'inline-block');       
//			$(color_input).minicolors('disabled', true);
			
		} else {
			
			// Not yet selected
			$(this).addClass("selected");
			$(this).css('color', color);
			
            $(color_input).parent().css('display', 'inline-block');
			//$(color_input).minicolors('disabled', false);
		}

		fetch_data_from_db_for_selections();
		
	});
	
	resize_data_content();
	
	hide_overlay();
}
