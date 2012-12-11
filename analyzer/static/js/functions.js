$(document).ready(function(){
	
	max_width = Math.max.apply(null, [$("#devices").width(), $("#kernels").width()]);
	$("#devices").width(max_width);
	$("#kernels").width(max_width);
/*	
	$("#result_data").width(
		$(window).width() - $("#result_data").offset().left - 20	
	);
*/	
	
	$(".entry").click(function(){
		$(this).toggleClass("selected");
		
		// Trigger a data update
		data_update();
		
	});
	
	$("#plot_btn").click(plot());
	
});

function data_update() {
	
	selected_kernel_ids = [];
	selected_device_ids = [];
	
	$.each($(".entry.selected"), function(index, value) {
	
		id = $(value).html().split(":", 1);
		
		if ($(value).hasClass("kernel")) {
			selected_kernel_ids += id;
		}
		
		if ($(value).hasClass("device")) {
			selected_device_ids += id;
		}
	
	});
	
	// Trigger request to DB
	$.getJSON('/get_data', {
		devices: selected_device_ids,
		kernels: selected_kernel_ids
	}, function(data) {
		
		//data.result.unshift(['Start-time', 'Run-time', 'Thread ID']);
		data.result.unshift(['Thread', '', '', '', '']);
		
		var data_table = google.visualization.arrayToDataTable(data.result);
		
		
		var table = new google.visualization.Table(document.getElementById('data_table'));
        table.draw(data_table, {showRowNumber: true});
		
		/*
		table = '<table border="1"><tr>';
		table += '<td>#</td><td>Start time</td><td>Run-time</td><td>Thread ID</td>';
		table += '</tr>';
		
		$.each(data.result, function(i, row) {			
			table += '<tr>';
			table += '<td>' + i + '</td><td>' + row[0] + '</td><td>' + row[1] + '</td><td>' + row[2] + '</td>';  
			table += '</tr>';
		});
			
		table += '</tr></table>'
			
		$("#data_table").html(table);
		*/
		
        plot(data_table)
		
	});	
}

function plot(data_table) {
	
	//var data_table = new google.visualization.DataTable(data);
	//var data_table = google.visualization.arrayToDataTable(data);
	
	new google.visualization.CandlestickChart(document.getElementById("visualization")).draw(data_table, {
		
		title: "Test Plot",
		width: 600,
		height: 400,
		vAxis: {title: "Thread"},
		hAxis: {title: "Time"}
		
	});
	
	//$.each(data, function(i, row) {
		
		//data_table.addRow(row)
		
	//});
	
	
	
/*
	  // Create and populate the data table.
	  var data = google.visualization.arrayToDataTable([
	    ['Year', 'Austria', 'Bulgaria', 'Denmark', 'Greece'],
	    ['2003',  1336060,    400361,    1001582,   997974],
	    ['2004',  1538156,    366849,    1119450,   941795],
	    ['2005',  1576579,    440514,    993360,    930593],
	    ['2006',  1600652,    434552,    1004163,   897127],
	    ['2007',  1968113,    393032,    979198,    1080887],
	    ['2008',  1901067,    517206,    916965,    1056036]
	  ]);

	  // Create and draw the visualization.
	  new google.visualization.BarChart(document.getElementById('visualization')).
	      draw(data,
	           {title:"Yearly Coffee Consumption by Country",
	            width:600, height:400,
	            vAxis: {title: "Year"},
	            hAxis: {title: "Cups"}}
	      );
*/	  
}