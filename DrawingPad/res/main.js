function loadMessageBox(caption, text)
{
	document.getElementById("caption").innerHTML = caption;
	document.getElementById("message_con").innerHTML = text;
	document.getElementById("mesbox").style.transform = "none";
	document.getElementById("mesbox").style.WebkitTransform = "none";
	document.getElementById("mesbox").style.MsTransform = "none";
	document.getElementById("mesbox").style.MozTransform = "none";
	document.getElementById("mesbox").style.OTransform = "none";
}

function closeMessageBox()
{
	document.getElementById("mesbox").style.transform = "translate(-150%, -150%) rotate(360deg)";
	document.getElementById("mesbox").style.WebkitTransform = "translate(-150%, -150%) rotate(360deg)";
	document.getElementById("mesbox").style.MsTransform = "translate(-150%, -150%) rotate(360deg)";
	document.getElementById("mesbox").style.MozTransform = "translate(-150%, -150%) rotate(360deg)";
	document.getElementById("mesbox").style.OTransform = "translate(-150%, -150%) rotate(360deg)";	
}

function loadAJAXPost(location, data, callback)
{
	var xhr = new XMLHttpRequest();
	xhr.onreadystatechange = function()
	{
		if(xhr.readyState = 4)
		{
			callback(xhr.responseText);
		}
	};

	xhr.open("POST", location, true);
	xhr.setRequestHeader("Content-type","application/x-www-form-urlencoded");
	xhr.timeout = 4000;
	xhr.send(data);
}

window.onbeforeunload = function()
{
	loadAJAXPost("/unload", "?s=" + encodeURI(document.getElementById("sesname").value), function(x) {});
};

var ctx;
var send = "";
var liney = 0;

function sendRequest()
{
	if(document.getElementById("sesname").value == "")
	{
		loadMessageBox("Session Empty!", "Changes lost! Please fill out the session name, up at the top! :)");
		send = "";
		return;
	}
	document.getElementById("dbg").innerHTML = "Session Empty!";
	var data = send;
	data = "req=" + encodeURI(send) + "&ses=" + encodeURI(document.getElementById("sesname").value);
	document.getElementById("dbg").innerHTML = data;
	loadAJAXPost("/project", data, function(x) {});
	send = "";
}

function draw()
{
	var ctx = document.getElementById("main_canvas").getContext("2d");
	for (var i = 0; i < 640; i++){
		for (var j = 0; j < 640; j++){
			ctx.fillStyle = 'rgb(' + Math.floor(255 - (255 / 640) * i) + ',' + Math.floor( 255 - (255 / 640) * j) + ', 0)';
			ctx.fillRect(j * 1, i * 1, 1, 1);
		}
	}
}

window.onload = function()
{
	var canvas = document.getElementById("main_canvas");
	canvas.width = $(window).width() * 0.8;
	canvas.height = $(window).height() * 0.8;
	var color = "#98BF21";
	if(canvas)
	{
		var isDown = false;
		var ctx = canvas.getContext("2d");
		var canvasX, canvasY;
		ctx.lineWidth = 5;

		$(canvas).mousedown(function(e)
		{
			//Let's set that we are holding our finger on the touchpad left button
			isDown = true;
			//Start the path for us to draw using.
			ctx.beginPath();
			//Get the position of the hit
			canvasX = e.pageX - canvas.offsetLeft;
			canvasY = e.pageY - canvas.offsetTop;
			//Move the cursor to the starting location
			ctx.moveTo(canvasX, canvasY);

			//Set the fill color
			ctx.fillStyle = color;
			//Draw a rectangle
			ctx.fillRect(canvasX, canvasY, 5, 5);
			send = send + " begpath " + color + " " + canvas.width + " " + canvas.height + " " + canvasX + " " + canvasY;
		}).mousemove(function(e)
		{
			//Check if the mouse is down
			if(isDown != false)
			{
				liney++;
				if(liney % 2 == 0)
				{
					//Set the color to our right color
					ctx.strokeStyle = color;
					//Get the touch position
					canvasX = e.pageX - canvas.offsetLeft;
					canvasY = e.pageY - canvas.offsetTop;
					//Draw a line to the position we are at now
					ctx.lineTo(canvasX, canvasY);
					//Draw everything!
					ctx.stroke();
					send = send + " point " + canvasX + " " + canvasY;
				}
			}
		}).mouseup(function(e)
		{
			//No longer drawing anymore
			isDown = false;
			//Close the path
			ctx.closePath();
			send = send + " endpath ";
			sendRequest();
		});
	}
};
