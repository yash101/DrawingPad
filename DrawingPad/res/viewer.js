var curqd = 0;
var inProgress = false;
var sesname = null;
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
	document.getElementById("status_light").style.backgroundColor = "blue";
	var xhr = new XMLHttpRequest();
	xhr.onreadystatechange = function()
	{
		document.getElementById("status_light").style.backgroundColor = "green";
		document.getElementById("servcon").style.backgroundColor = "green";
		if(xhr.readyState = 4)
		{
			callback(xhr.responseText);
		}
	};

	xhr.onerror = function()
	{
		document.getElementById("servcon").style.backgroundColor = "red";
		document.getElementById("status_light").style.backgroundColor = "red";
	}

	xhr.open("POST", location, true);
	xhr.setRequestHeader("Content-type","application/x-www-form-urlencoded");
	xhr.timeout = 4000;
	xhr.send(data);
	document.getElementById("status_light").style.backgroundColor = "sky-blue";
}


function loadAJAXPostErrorHandler(location, data, callback)
{
	var xhr = new XMLHttpRequest();
	xhr.onreadystatechange = function()
	{
		if(xhr.readyState = 4)
		{
			callback(xhr.responseText);
		}
	};

	xhr.onerror = callback;

	xhr.open("POST", location, true);
	xhr.setRequestHeader("Content-type","application/x-www-form-urlencoded");
	xhr.timeout = 4000;
	xhr.send(data);
}

function loadAJAXGet(location, callback)
{
	var xhr = new XMLHttpRequest();
	xhr.onreadystatechange = function()
	{
		if(xhr.readyState == 4)
		{
			callback(xhr.responseText);
		}
	};

	xhr.open("POST", location, true);
	xhr.timeout = 4000;
	xhr.send();
}

function getSessionHandler()
{
	return "ses=" + encodeURI(sesname.value);
}

function sleep(milliseconds)
{
	var start = new Date().getTime();
	while(true)
	{
		if ((new Date().getTime() - start) > milliseconds)
		{
			break;
		}
	}
}

var canvas = null;
var ctx = null;
var heightSet = false;
var strokeRad = 5;

function parseRequest(c)
{
	var cmd = c.split(' ');
	for(var i = 0; i < cmd.length; i++)
	{
		if(cmd[i] == "begpath")
		{
			ctx.beginPath();
			i++;
			ctx.lineCap = cmd[i];

			i++;
			ctx.fillStyle = decodeURI(cmd[i]);
			ctx.strokeStyle = decodeURI(cmd[i]);

			if(!heightSet)
			{
				i++;
				canvas.width = parseInt(cmd[i]);
				i++;
				canvas.height = parseInt(cmd[i]);
				heightSet = true;
			}
			else
			{
				i = i + 2;
			}
			i++;
			var canvasX = parseInt(cmd[i]);
			i++;
			var canvasY = parseInt(cmd[i]);
			i++;
			strokeRad = parseInt(cmd[i]);

			ctx.moveTo(canvasX, canvasY);
			ctx.lineWidth = strokeRad;
			ctx.fillRect(canvasX - 2 - (strokeRad / 2), canvasY - 2 - (strokeRad / 2), strokeRad, strokeRad);
		}

		if(cmd[i] == "point")
		{
			i++;
			var canvasX = parseInt(cmd[i]);
			i++;
			var canvasY = parseInt(cmd[i]);
			ctx.lineTo(canvasX, canvasY);
			ctx.stroke();
		}

		if(cmd[i] == "endpath")
		{
			ctx.closePath();
		}

		if(cmd[i] == "clear")
		{
			ctx.clearRect(0, 0, document.getElementById("main_canvas").width, document.getElementById("main_canvas").height);
		}
	}
}

function sesname_change()
{
	document.getElementById("sesname").value = document.getElementById("sesname").value.toUpperCase();
	ctx.clearRect(0, 0, document.getElementById("main_canvas").width, document.getElementById("main_canvas").height);
	curqd = 0;
	loadInfo("Session Name Updated:<h3>[" + document.getElementById("sesname").value + "]!</h3>");
}

var renderer_handler = null;
function render()
{
	if(document.getElementById("sesname").value != "")
	{
		if(!inProgress)
		{
			inProgress = true;
			var newqd = 0;
			var fetching = false;
			loadAJAXPost("/numqueues", getSessionHandler(), function(x)
			{
				document.getElementById("dbg").innerHTML = "Sent request for queue depth!";
				newqd = parseInt(x);
				document.getElementById("dbg").innerHTML = "Recieved: " + x;
				for(curqd; curqd < newqd; curqd++)
				{
					while(fetching)
					{
						sleep(1);
					}

					fetching = true;

					loadAJAXPost("/fetch", getSessionHandler() + "&index=" + encodeURI(curqd), function(y)
					{
						document.getElementById("dbg").innerHTML = y;
						parseRequest(y);
					});

					fetching = false;
					curqd++;
					break;
				}
			});
			inProgress = false;
		}
	}
}

function getInstructions()
{
	loadAJAXPost("/instructions", "i=viewer", function(x)
	{
		loadMessageBox("Instructions", x);
	});
}

window.onload = function()
{
	if(pid != "")
	{
		document.getElementById("sesname").value = decodeURI(pid);
	}
	canvas = document.getElementById("main_canvas");
	sesname = document.getElementById("sesname");
	ctx = canvas.getContext("2d");
	loadAJAXPost("/instructions", "i=viewer", function(x)
	{
		loadMessageBox("Instructions", x);
	});

	renderer_handler = setInterval(render, 200);
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

var fscreen = false;
function fullscreen()
{
	if(fscreen)
	{
		fscreen = false;
		document.getElementById("main_canvas").style.top = "2%";
		document.getElementById("main_canvas").style.left = "2%";
		document.getElementById("main_canvas").style.width = "96%";
		document.getElementById("main_canvas").style.height = "96%";
	}
	else
	{
		fscreen = true;
		document.getElementById("main_canvas").style.top = "10%";
		document.getElementById("main_canvas").style.left = "10%";
		document.getElementById("main_canvas").style.width = "80%";
		document.getElementById("main_canvas").style.height = "80%";
	}
}