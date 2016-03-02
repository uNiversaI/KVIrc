function detect() {
	var OSName="Unknown OS";
	if (navigator.appVersion.indexOf("Win")!=-1) OSName="Windows";
	if (navigator.appVersion.indexOf("Mac")!=-1) OSName="MacOS";
	if (navigator.appVersion.indexOf("X11")!=-1) OSName="UNIX";
	if (navigator.appVersion.indexOf("Linux")!=-1) OSName="Linux";

	console.log(OSName);
	document.getElementById('downloadbtn').innerHTML = '<i class="fa fa-download">&nbsp;Download for ' + OSName + '</i>';
	document.getElementById('downloadbtn1').innerHTML = '<i class="fa fa-download">&nbsp;Download for ' + OSName + '</i>';
	document.getElementById('downloadbtn2').innerHTML = '<i class="fa fa-download">&nbsp;Download for ' + OSName + '</i>';

}