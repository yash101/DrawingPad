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

function toolTip(x)
{
	document.getElementById("tooltip").innerHTML = x;
}

function loadInfo(x)
{
}

function loadAJAXPost(location, data, callback)
{
	var xhr = new XMLHttpRequest();
	xhr.onreadystatechange = function()
	{
		document.getElementById("servcon").style.backgroundColor = "green";
		if(xhr.readyState = 4)
		{
			callback(xhr.responseText);
		}
	};

	xhr.onerror = function()
	{
		document.getElementById("servcon").style.backgroundColor = "red";
	}

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
var canvas;
var send = "";
var liney = 0;

function sendRequest()
{
	if(document.getElementById("sesname").value == "")
	{
		loadMessageBox("Session Empty!", "Changes lost! Please fill out the session name, up at the top! :)");
		send = "";
		ctx.clearRect(0, 0, document.getElementById("main_canvas").width, document.getElementById("main_canvas").height);
		return;
	}
	var data = send;
	data = "req=" + encodeURI(send) + "&ses=" + encodeURI(document.getElementById("sesname").value);
	loadAJAXPost("/project", data, function(x) {});
	send = "";
}

function clearBoard()
{
	ctx.clearRect(0, 0, document.getElementById("main_canvas").width, document.getElementById("main_canvas").height);
	send = send + " clear ";
	sendRequest();
}

var keep_alive = setInterval(function()
{
	var sesbox = document.getElementById("sesname");
	if(sesbox.value != "")
	{
		loadAJAXPost("/keepalive", "ses=" + encodeURI(sesbox.value), function(x) { });
	}
}, 4000);

window.onload = function()
{
	loadAJAXPost("/instructions", "i=presenter", function(x)
	{
		loadMessageBox("Instructions", x);
	});
	canvas = document.getElementById("main_canvas");
	canvas.width = $(window).width() * 0.8;
	canvas.height = $(window).height() * 0.8;

	if(canvas)
	{
		var isDown = false;

		ctx = canvas.getContext("2d");

		var canvasX, canvasY;

		$(canvas).mousedown(function(e)
		{
			//Let's set that we are holding our finger on the touchpad left button
			isDown = true;
			//Start the path for us to draw using.
			ctx.beginPath();
			//Get the position of the hit
			canvasX = e.pageX - canvas.offsetLeft + 12;
			canvasY = e.pageY - canvas.offsetTop + 12;
			//Move the cursor to the starting location
			ctx.moveTo(canvasX, canvasY);

			//Set the fill color
			ctx.fillStyle = document.getElementById("penCol").value;
			//Draw a rectangle
			ctx.fillRect(canvasX - 2, canvasY - 2, document.getElementById("penRad").value, document.getElementById("penRad").value);
			ctx.lineWidth = document.getElementById("penRad").value;
			send = send + " begpath " + document.getElementById("penCol").value + " " + canvas.width + " " + canvas.height + " " + canvasX + " " + canvasY + " " + document.getElementById("penRad").value;
		}).mousemove(function(e)
		{
			//Check if the mouse is down
			if(isDown != false)
			{
				liney++;
				if(liney % parseInt(document.getElementById("lineyness").value) == 0)
				{
					//Set the color to our right color
					ctx.strokeStyle = document.getElementById("penCol").value;
					//Get the touch position
					canvasX = e.pageX - canvas.offsetLeft + 12;
					canvasY = e.pageY - canvas.offsetTop + 12;
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

function sesbox_change()
{
	document.getElementById("sesname").value = document.getElementById("sesname").value.toUpperCase();
}

var ibox = null;
function loadInfo(x)
{
	var con = document.getElementById("infobox");
	con.style.display = "block";
	con.innerHTML = x;
	clearInterval(ibox);
	ibox = setInterval(closeInfo, 1000);
}

function closeInfo()
{
	clearInterval(ibox);
	var con = document.getElementById("infobox");
	con.style.display = "none";
}