@extends('master')
@section('title', 'Plot Time Vs Temp')

@section('content')

<head>

  <!-- http-equiv="refresh" content="1800;url=/plotTemps" causes the page to be
      refreshed every 1800 seconds (30 minutes) and to use a GET instead of a POST. -->
  <meta charset="UTF-8" http-equiv="refresh" content="1800;url=/plotTemps">

  <style type="text/css">

    .dropbbb {
      border-style: solid;
      border-width: thin;
      border-color: gray;
      border-radius: 4px;
      padding: 2px 4px;
      background-color: #f1f1f1;
    }

    .dropbtn {
      position: relative;
      display: inline-block;
    }

    .dropbtn-content {
      display: none;
      position: absolute;
      background-color: #f1f1f1;
      min-width: 90px;
      box-shadow: 0px 8px 16px 0px rgba(0,0,0,0.2);
      z-index: 1;
      text-align: center;
    }

    .dropbtn-content a {
      color: black;
      padding: 4px 16px;
      text-decoration: none;
      display: block;
    }

    .dropbtn-selected {
      background-color: #56e35c;
    }

    .dropbtn-content a:hover {background-color: #ddd;}

    .dropbtn:hover .dropbtn-content {display: block;}

    .dropbtn:hover .dropbbb {background-color: #56e35c;}

    .dropdown {
      position: relative;
      display: inline-block;
      /*margin: 0px 2px;*/
      /*margin-right: 20px;*/
      background-color: rgba(0,0,0,0.001);
    }

    .dropdown-content {
      display: none;
      position: absolute;
      min-width: 180px;
      box-shadow: 0px 8px 16px 0px rgba(0,0,0,0.2);
      padding: 12px 16px;
      border-style: solid;
      border-width: thin;
      border-color: gray;
      border-radius: 4px;
      z-index: 1;
    }

    .timeShift {
      border-style: solid;
      border-width: thin;
      border-color: gray;
      border-radius: 4px;
    }

    #startTimeData {
      margin: 4px 5px;
      float:left;
    }

    #chnlSelect, #prgmSelect {
      display: none;
      height: 200px; overflow: auto; width: 100px; border: 1px solid #000;
      list-style-type: none; margin: 0; padding: 0; overflow-x: hidden;
      float:right;
      /*margin: 0px 2px;*/
    }

    #chnlColor {
      margin: 0px 5px;
      float:left;
      display: none;
    }

    #prgmColor {
      margin: 0px 5px;
      float:left;
      display: none;
    }

    #settingsBtn {
      position: relative;
      float: right;
      margin-right: 3px;
      border-style: solid;
      border-width: thin;
      border-color: gray;
      border-radius: 4px;
      padding: 2px 4px;
      background-color: #f1f1f1;
      margin-top: 4px;
    }

    #settingsBtn:hover {background-color: #56e35c;}

    #setMax {
      width: 50px;
    }

    #setMin {
      width: 50px;
      margin: 4px;
    }

    #submitBtn{
      border-style: solid;
      border-width: thin;
      border-color: gray;
      border-radius: 4px;
      background-color: #f1f1f1;
      padding: 2px 4px;
      margin-top: 4px;
    }

    #startDate{
      width: 85px;
      text-align: center;
      border-style: solid;
      border-width: thin;
      border-color: gray;
      border-radius: 4px;
      background-color: #f1f1f1;
      padding: 2px 4px;
    }

    #chnlHeader, #prgmHeader {
      width: 70px;
      text-align: center;
      border-style: solid;
      border-width: thin;
      border-color: gray;
      border-radius: 4px;
      background-color: #f1f1f1;
      padding: 2px 4px;
    }



    #submitBtn:hover, #startDate:hover {background-color: #56e35c;}


    label { display: block; color: WindowText; background-color: Window; margin: 0; padding: 0px 0px 0px 2px; width: 100%; }
    label:hover { background-color: #CBF6D2; color: HighlightText; }

    /* The Modal (background) */
    .PEmodal {
      display: none; /* Hidden by default */
      position: fixed; /* Stay in place */
      z-index: 1; /* Sit on top */
      padding-top: 100px; /* Location of the box */
      left: 0;
      top: 0;
      width: 100%; /* Full width */
      height: 100%; /* Full height */
    	/*height: 200px; /* Full height */
      overflow: auto; /* Enable scroll if needed */
      background-color: rgb(0,0,0); /* Fallback color */
      background-color: rgba(0,0,0,0.4); /* Black w/ opacity */
    }

    /* Modal Content */
    .PEmodal-content {
      background-color: #fefefe;
      margin: auto;
      padding: 20px;
      border: 1px solid #888;
      width: 80%;
    }

    /* The Close Button */
    .PEclose {
      color: #aaaaaa;
      float: right;
      font-size: 28px;
      font-weight: bold;
    }

    .PEclose:hover,
    .PEclose:focus {
      color: #000;
      text-decoration: none;
      cursor: pointer;
    }

    .PEWarning {
      border-width: 2px;
      border-radius: 6px;
      border-color: red;
    }

    .PECenter {
      display: flex;
      justify-content: center;
      text-align: center;
    }

    .PEBtns {
      border-width: 1px;
      border-radius: 4px;
      border-color: black;
      font-family: Arial, Helvetica, Sans-serif;
      font-size: 16px;
      margin-top: 10px;
      margin-left: 5px;
      min-width: 100px;
    }

  </style>

</head>

<link rel="stylesheet" type="text/css" href="/css/jquery.datetimepicker.css"/ >
<script src="/js/jquery.datetimepicker.full.min.js"></script>
<script src="/js/PELocalStorage.js"></script>

<body>

<!--<br>-->
<!-- Create the form which holds the configuration for plotting time vs temperature. -->
<form id="tvtConfigForm" method="post">

  <!-- Display any errors. -->
  @foreach ($scErrors as $error)
    <p class="alert alert-danger">{{ $error }}</p>
  @endforeach

  <!-- The next line submits the token required by Laravel -->
  <input type="hidden" name="_token" value="{!! csrf_token() !!}">
  <!-- Create a hidden input to hold the chnls. -->
  <input type="hidden" id="theChnls" name="theChnls">
  <!-- Create a hidden input to hold the session configuration. -->
  <input type="hidden" id="peLS" name="peLS">

  <!-- Configures the controls used to select the starting date. -->
	<div id="startTimeData">
		Start Date: <!-- Create the start date control. -->
    <!-- This is used with the JQuery datetimepicker to get the start date. -->
    <input id="startDate" type="text" >
    <!-- Create the "Start Hour" drop down menu to select the start hour. -->
    <div class="dropbtn">
      <button id="startHour" class="dropbbb">Start Hour:</button>
      <div id="hours" class="dropbtn-content"></div>
    </div>
    <!-- Create the "Shift By:" drop down menu to shift the start time. -->
    <div class="dropbtn">
      <button class="dropbbb">Shift by:</button>
      <div class="dropbtn-content">
        <a href="javascript:shiftBy(-1);">- 1 hour</a>
        <a href="javascript:shiftBy(1)">+ 1 hour</a>
        <a href="javascript:shiftBy(-24);">- 1 day</a>
        <a href="javascript:shiftBy(24)">+ 1 day</a>
        <a href="javascript:shiftBy(-168);">- 1 week</a>
        <a href="javascript:shiftBy(168)">+ 1 week</a>
      </div>
    </div>
    <!-- Create the "Span:" drop down menu to alter the number of hours plotted. -->
    <div class="dropbtn">
      <button id="span" class="dropbbb">Span:</button>
      <div class="dropbtn-content">
        <a href="javascript:changeSpan(4);">4 hours</a>
        <a href="javascript:changeSpan(8)">8 hours</a>
        <a href="javascript:changeSpan(12);">12 hours</a>
        <a href="javascript:changeSpan(16)">16 hours</a>
        <a href="javascript:changeSpan(20);">20 hours</a>
        <a href="javascript:changeSpan(24)">1 day</a>
        <a href="javascript:changeSpan(36)">1.5 days</a>
        <a href="javascript:changeSpan(48)">2 days</a>
        <a href="javascript:changeSpan(72)">3 days</a>
        <a href="javascript:changeSpan(96)">4 days</a>
        <a href="javascript:changeSpan(144)">6 days</a>
        <a href="javascript:changeSpan(192)">8 days</a>
        <a href="javascript:changeSpan(336)">2 weeks</a>
        <a href="javascript:changeSpan(672)">4 weeks</a>
      </div>
    </div>

  </div> <!-- The end of the time controls. -->

  <div class="dropdown">  <!-- Creates the drop down control for selecting the channels and programs to plot. -->
    <div onmouseover="displayChnls()" onmouseout="hideChnls(event)" id="chnlHeader">Channels</div>
    <ul onmouseout="hideChnls(event)" class="dropdown-content" id="chnlSelect"></ul>
  </div>
  <div class="dropdown">  <!-- Creates the drop down control for selecting the programs to plot. -->
    <div onmouseover="displayPrgms()" onmouseout="hidePrgms(event)" id="prgmHeader">Program</div>
    <ul onmouseout="hidePrgms(event)" class="dropdown-content" id="prgmSelect"></ul>
  </div>

	<input id="submitBtn" type="submit" value="Update">

  <button id="settingsBtn" type="button">Settings</button>

  <div id="chnlColor">
    Color:
    <!-- The name is important so that the chnl's color can be passed to and from the input. -->
    <input id="chnlColorPicker" type="color" name="chnlColorInput" value="#ff0000">
  </div>

  <div id="prgmColor">
    Color:
    <!-- The name is important so that the chnl's color can be passed to and from the input. -->
    <input id="prgmColorPicker" type="color" name="prgmColorInput" value="#ff0000">
  </div>

</form>

<div>
  <h3 style="text-align:center;">Plot Time vs. Temperature</h3>
  <!-- Create a canvas for chart.js to draw the time vs temperture. -->
	<canvas id="myChart" width=400px height=200px></canvas>
</div>

<!-- Modal dialog for getting the grid settings. -->
<div id="settingsModal"  class="PEmodal">
    <!-- Modal content-->
  <div id="PE-content" class="PEmodal-content">
    <span id="closeSettings" class="PEclose">&times;</span>
    <h2>Setting</h2>
    Grid Maximum:
    <input id="setMax" type="number">
    <p>
      Grid Minimum:
      <input id="setMin" type="number">
    </p>
    Units:
    <p id="unit_place"></p>
    <button id="btnCloseSettings" type="button">Close</button>
  </div>
</div>

<!-- The modal dialog to send a warning about local storage to the user. -->
<div id="warningStorageModal" class="PEmodal">
  <!-- modal content -->
  <div class="PEmodal-content PEWarning">
    <div class="PECenter">
      <h2>This application would like to store the following settings:</h2>
    </div>
    <div class="PECenter">
      <h4>
        <p>Channel & Relay colors</p>
        <p>Channels & Relays to plot</p>
        <p>Graphing parameters for Channels & Relays</p>
      </h4>
    </div>
    <div class="PECenter">
      <button id="warningStorageOKBtn" class="PEBtns">OK</button>
      <button id="warningStorageCancelBtn" class="PEBtns">Don't store</button>
    </div>
  </div>
</div><!-- End of the dialog to get a yes or no response. -->

</body>

<script src="js/moment.min.js"></script>
<script src="js/Chart.bundle.min.js"></script>

<script>
///////////////////Global/Variables//////////////////////////////////////////
  // load specifies, when true, to attempt to load local variables on the user's device.
  var load = <?php echo json_encode($load); ?>;
  var peLS = <?php echo json_encode($peLS); ?>;// Contains this session's configuration.
  if(load == false) {                 // Is this page already loaded and just being updated?
    allTvtData = <?php echo json_encode($tvtData); ?>;// Yes. Put the tvt data into a json string.
    startMoment = new moment(peLS['PE_TVT_StartTime']);// Get the start moment.
    if(allTvtData.length != 0) {      // Is there tvt data?
      tvtData = allTvtData[0];        // Yes. Get the tvt data.
    } // End of condition: There is tvt data.
  } // End of condition: The plot tvt page is being plotted.
  var theStartHour = peLS['PE_TVT_StartTime'].substr(11, 2);  // The 11th and 12th characters contain the hour.
  var theChnls = <?php echo json_encode($theChnls); ?>; // Access the list of all the systems chnls.
  var colorPickerDisplayed = false;                     // The colorPicker has not been displayed yet.
///////////////End/Global/Variables//////////////////////////////////////////

  document.getElementById("closeSettings").onclick = function() {   // Create an event for when the settings close.
    document.getElementById("settingsModal").style.display = "none";// Hide the settings dialog.
  }

  document.getElementById("btnCloseSettings").onclick = function() {  // Create an event for when the settings close.
    document.getElementById("settingsModal").style.display = "none";  // Hide the settings dialog.
  }

  function createRadioBtn(name, value, text) {
    var label = document.createElement("label");// Create a label.
    var radio = document.createElement("input");// Create an input for a radio button.
    radio.type = "radio";             // The type of input is a radio button.
    radio.name = name;                // Set the button's name.
    radio.id = name + value;          // Set the button's id.
    if(value == peLS['PE_TVT_Units']) {//
      radio.checked = true;
    }
    radio.value = value;
    label.appendChild(radio);         // Add the radio button to its label.
    label.appendChild(document.createTextNode(text));
    return label;
  }

  function displayChnls() {
    var chnlList = document.getElementById("chnlSelect");// Get the chnl list.
    chnlList.style.display = "block";     // Display the chnl list.
    chnlList.style.position = "absolute"; // Use absolute positioning.
    document.getElementById("chnlHeader").style.backgroundColor = "#56e35c";// Light green background.
  }

  function displayPrgms() {
    var prgmList = document.getElementById("prgmSelect");// Get the program list.
    prgmList.style.display = "block";     // Display the program list.
    prgmList.style.position = "absolute"; // Use absolute positioning.
    document.getElementById("prgmHeader").style.backgroundColor = "#56e35c";// Light green background.
  }

  function hideChnls(event) {
    // Set the background color to the value it has when chnls are not being selected.
    document.getElementById("chnlHeader").style.backgroundColor = "#f1f1f1";
    var fromIdOK = false;             // Only respond if the event is from an id with
                                      // color, relay, or chkbx in the id.
    // Are the fromElement and relatedTarget elements defined.
    if(event.fromElement == null || event.relatedTarget == null) {
      return;                         // No. there is no way to respond.
    } // End of condition: fromElement and relatedTarget don't exist.

    if(event.fromElement.id.substr(0, 5) == "color") {// Is this event from a color picker?
      fromIdOK = true;                // Yes. It is OK to hide the chnl list.
    } else if(event.fromElement.id.substr(0, 4) == "chnl") {// Is this event from a chnl element?
      fromIdOK = true;                // Yes. It is OK to hide the chnl list.
    } else if(event.fromElement.id.substr(0, 5) == "chkbx") {// Is this event from a chnl checkbox?
      fromIdOK = true;                // Yes. It is OK to hide the chnl list.
    } // End of conditions: The event is from an element that requires hiding the chnls.
    if(fromIdOK == false) {           // Should this event be ignored?
      return;                         // Yes.
    } // End of condition: This event should be ignored.
    if(colorPickerDisplayed == true) {// Is the colorPicker being displayed?
      return;                         // Yes. Don't hide the chnl list.
    } // End of condition: The colorPicker is displayed.
    if(event.relatedTarget.id.substr(0, 5) == "color") {// Is the next element a colorPicker?
      return;                         // Yes. Don't hide the chnl list.
    } else if(event.relatedTarget.id.substr(0, 4) == "chnl") {// Is the next element a chnl?
      return;                         // Yes. Don't hide the chnl list.
    } else if(event.relatedTarget.id.substr(0, 5) == "chkbx") {// Is the next element a chnl checkbox?
      return;                         // Yes. Don't hide the chnl list.
    }// End of conditions: The chnl list should still be displayed
    // The chnl list should be hidden.
    document.getElementById("chnlSelect").style.display = "none";  // Hide the chnl list.
  } // End of function hideChnls.

  function hidePrgms(event) {
    // Set the background color to the value it has when prgms are not being selected.
    document.getElementById("prgmHeader").style.backgroundColor = "#f1f1f1";
    var fromIdOK = false;             // Only respond if the event is from an id with
                                      // color, program, or chkbx in the id.
    // Are the fromElement and relatedTarget elements defined.
    if(event.fromElement == null || event.relatedTarget == null) {
      return;                         // No. there is no way to respond.
    } // End of condition: fromElement and relatedTarget don't exist.
    if(event.fromElement.id.substr(0, 4) == "prgm") {// Is this event from a prgm element?
      fromIdOK = true;                // Yes. It is OK to hide the prgm list.
    }
    if(fromIdOK == false) {           // Should this event be ignored?
      return;                         // Yes.
    } // End of condition: This event should be ignored.
    if(colorPickerDisplayed == true) {// Is the colorPicker being displayed?
      return;                         // Yes. Don't hide the prgm list.
    } // End of condition: The colorPicker is displayed.
    if(event.relatedTarget.id.substr(0, 4) == "prgm") {// Is the next element a prgm?
      return;                         // Yes. Don't hide the prgm list.
    } // End of conditions: The prgm list should still be displayed
    // The prgm list should be hidden.
    document.getElementById("prgmSelect").style.display = "none";  // Hide the chnl list.
  } // End of function hidePrgms.

  function displayColorPicker() {
    colorPickerDisplayed = true;
  }

function configElements() {
  if(load == true) {                  // The plot TVT page is being loaded, not plotted.
    checkLocalStorage();              // Check for a configuration in local storage.
  } else { // End of condition: The plot TVT page is being loaded, not plotted.
    if(peLS['isUsed'] == 'unknown') { // Condition: Local storage been evaluated. Is local storage unknow.
      // Yes. Use the warningDlg to ask the user if they would like to use local storage.
      var warningDlg = document.getElementById("warningStorageModal");
      warningDlg.style.display = "block"; // Display the warning dialog.
    } // End of condition: Unknown usage of local storage.
  } // End of condiiton: load.

  var theStartDate = peLS['PE_TVT_StartTime'].substr(0, 10);// The first 10 characters of the start time are the date.
  theStartHour = peLS['PE_TVT_StartTime'].substr(11, 2);  // The 11th and 12th characters contain the hour.
  if(theStartHour.substr(0,1) == "0") { // Does the hour start with a zero?
    theStartHour = peLS['PE_TVT_StartTime'].substr(12, 1);// Yes. The zero must be removed for the startHour control to work.
  }                                     // The start hour is in the right format for the startHour control.
  // Create a string containg the date in US format.
  var usDate = peLS['PE_TVT_StartTime'].substr(5, 2) + "-";// The US date date starts with the month.
  usDate += peLS['PE_TVT_StartTime'].substr(8, 2) + "-";// It then contains the day.
  usDate += peLS['PE_TVT_StartTime'].substr(0, 4);// Finally, it contains the year.
  sdElement = document.getElementById("startDate");// Get the start date element.
  sdElement.value = usDate;         // Display the date using US format.
  jQuery('#startDate').datetimepicker({ // Configure the datetimepicker.
    //  format:'d.m.Y H:i',
    format:'m-d-Y',                     // Set the display format to a US date with no time.
    //  inline:true,
    timepicker:false,                   // Don't show the timepicker.
    inline:false,                       // It is not to come up in drop down format.
    startDate:theStartDate,             // Set the date using ISO 8601 YYYY-MM-DD.
    onChangeDateTime:settingsChanged,   // Call an event handler if the user changes the date.
  }); // The datetimepicker is configured,
  for(var val = 0; val < 24; val++) {         // Loop through the valid start hours between 0 and 23.
    var link = document.createElement("a");   // Create a link for the start hour choice.
    // Create an attribute that causes the changeStartHour function to be called
    // when this link is clicked. The link passes the desired start hour
    // to the changeStartHour function.
    link.setAttribute('href', 'javascript:changeStartHour(' + val + ')');
    var text = document.createTextNode(val.toString());// Create its text.
    if(Number(theStartHour) == val) { // Is this the currently selected start hour?
      link.setAttribute('class', 'dropbtn-selected');// Yes. Change its appearance by chaninging its class.
    } // End of condition: This hour is the selected start hour.
    link.appendChild(text);                   // Add the text to the choice.
    document.getElementById("hours").appendChild(link);// Add this hour as a choice for the start hour.
  } // End of looping through the 24 hours of the day.
  document.getElementById("startHour").innerHTML = "Start Hour: " + theStartHour;
  var s = "Span: ";                   // The span drop down starts with the word Span:.
  var a = peLS['PE_TVT_Span'] + " hours";    // The default after Span: is the number of hours to plot.
  if(peLS['PE_TVT_Span'] > 192) {            // Are the number of hours 1.14 weeks?
    a = peLS['PE_TVT_Span'] / 168 + " weeks";// Yes. Humans prefer the number of weeks verses hours or days.
  } else if (peLS['PE_TVT_Span'] >= 24) {    // Are the number of hours at least 24?
    a = peLS['PE_TVT_Span'] / 24 + " days";  // Use days instead of hours.
  } // End of condition: at least 24 hours are being plotted.
  document.getElementById("span").innerHTML = s + a;// Set the Span controls text.
  var chnlList = document.getElementById("chnlSelect");// Create a variable to access the list of chnls.
  chnlList.style.display = "none";              // The list is not shown when it is created.
  var i = 0;                                    // Start with the first chnl.
  for(; i < theChnls.length; i++) {             // Go through all the chnls.
    var chnlItem = document.createElement("li");// Create a list item for this chnl.
    var called = document.createTextNode(theChnls[i].name);// Create a text node containing this chnl's name.
    var myLabel = document.createElement("label");// Create a label for this chnl.
    myLabel.setAttribute("id", "chnl" + i);     // Set the id for this chnl's label.
    myLabel.style.color = peLS['PE_Chnl_Color'][i];// Set the label's color to the chnl's color.
    var myInput = document.createElement("input");// Create an input for this chnl.
    myInput.setAttribute("type", "checkbox");   // Make this chnl's input a check box.
    myInput.setAttribute("name", "chkbx" + i);  // Give this chnl's check box a name.
    myInput.setAttribute("id", "chkbx" + i);    // Give this chnl's check box an id.
    var plot = peLS['PE_Chnl_Plot'][i];         // Get this chnl's plot flag.
    if(plot == 1) {                             // Is this chnl supposed to be plotted?
      myInput.setAttribute("checked", "true");  // Yes. Check its check box.
    }                                           // This chnl's check box has been set.
    var myInput2 = document.createElement("input");// Create an input to get color.
    myInput2.setAttribute("type", "color");     // The input type is color.
    myInput2.setAttribute("name", "color" + i); // The input name is color plus the chnl index.
    myInput2.setAttribute("id", "color" + i);   // The input id is color plus the chnl index.
    myInput2.setAttribute("value", peLS['PE_Chnl_Color'][i]);// Set this chnl's color.

    myInput2.setAttribute("class", "chnlColor");
    myInput2.addEventListener("input", updateColors);
    myInput2.addEventListener("click", displayColorPicker);

    myLabel.appendChild(myInput);
    myLabel.appendChild(myInput2);
    myLabel.appendChild(called);
    chnlItem.appendChild(myLabel);
    chnlList.appendChild(chnlItem);
  }
  var prgList = document.getElementById("prgmSelect");// Create a variable to access the list of chnls.
  prgList.style.display = "none";                   // The list is not shown when it is created.
  i = 0;                                            // Start with the first chnl.
  for(; i < theChnls .length; i++) {                // Go through all the chnls.
    var prgItem = document.createElement("li");     // Create a list item for this chnl.
    var prgCalled = document.createTextNode(theChnls[i].name);// Create a text node containing this chnl's name.
    var prgLabel = document.createElement("label"); // Create a label for this chnl.
    prgLabel.setAttribute("id", "prgm" + i);        // Set the id for this prg's label.
    prgLabel.style.color = peLS['PE_Prgm_Color'][i];// Set the label's color to the prg's color.
    var prgInput = document.createElement("input"); // Create an input for this prg.
    prgInput.setAttribute("type", "checkbox");      // Make this prg's input a check box.
    prgInput.setAttribute("name", "prgmChkbx" + i); // Give this prg's check box a name.
    prgInput.setAttribute("id", "prgmChkbx" + i);   // Give this prg's check box an id.
    var plot = peLS['PE_Prgm_Plot'][i];             // Get the plot flag.
    if(plot == 1) {              // Is this chnl supposed to be plotted?
      prgInput.setAttribute("checked", "true");  // Yes. Check its check box.
    }                                           // This chnl's check box has been set.
    var prgInput2 = document.createElement("input");
    prgInput2.setAttribute("type", "color");
    prgInput2.setAttribute("name", "prgmColor" + i);
    prgInput2.setAttribute("id", "prgmColor" + i);
    prgInput2.setAttribute("value", peLS['PE_Prgm_Color'][i]);
    prgInput2.setAttribute("class", "prgmColor");
    prgInput2.addEventListener("input", updatePrgmColors);
    prgInput2.addEventListener("click", displayColorPicker);
    prgLabel.appendChild(prgInput);
    prgLabel.appendChild(prgInput2);
    prgLabel.appendChild(prgCalled);
    prgItem.appendChild(prgLabel);
    prgList.appendChild(prgItem);
  }
  var myContent = document.getElementById("unit_place");
  var FBtn = createRadioBtn("degrees", "F", " Fahrenheit");
  myContent.appendChild(FBtn);
  var CBtn = createRadioBtn("degrees", "C", " Celsius");
  myContent.appendChild(CBtn);
  document.getElementById("setMax").value = peLS['PE_TVT_Max'];// Set the element for the highest temperture
  document.getElementById("setMin").value =peLS['PE_TVT_Min'];// Set the element for the lowest temperture
  if(load == true) {                  // Is the TVT page being loaded?
    // Yes. The page controller needs the locally stored variables to generate plot data.
    document.getElementById("submitBtn").click();// Update the page with the locally stored configuration.
    return;
  } // End of condition: The TVT page is being loaded (not updated).
  drawChart();
} // End of function configElements.

  function hslToHex(h, s, l) {
    h /= 360;
    s /= 100;
    l /= 100;
    let r, g, b;
    if (s === 0) {
      r = g = b = l; // achromatic
    } else {
      const hue2rgb = (p, q, t) => {
        if (t < 0) t += 1;
        if (t > 1) t -= 1;
        if (t < 1 / 6) return p + (q - p) * 6 * t;
        if (t < 1 / 2) return q;
        if (t < 2 / 3) return p + (q - p) * (2 / 3 - t) * 6;
        return p;
      };
      const q = l < 0.5 ? l * (1 + s) : l + s - l * s;
      const p = 2 * l - q;
      r = hue2rgb(p, q, h + 1 / 3);
      g = hue2rgb(p, q, h);
      b = hue2rgb(p, q, h - 1 / 3);
    }
    const toHex = x => {
      const hex = Math.round(x * 255).toString(16);
      return hex.length === 1 ? '0' + hex : hex;
    };
    return `#${toHex(r)}${toHex(g)}${toHex(b)}`;
  }

  function updateColors() {           // The user wants to change a chnl's color.
    var colorCtrls = document.getElementsByClassName("chnlColor");// Get the color controls.
    for(var chnlDex = 0; chnlDex < colorCtrls.length; chnlDex++) {// Loop through each chnl.
      var theNewColor = colorCtrls[chnlDex].value;    // Get this chnl's color from its color control.
      var theNum = colorCtrls[chnlDex].name.substr(5);// The number starts in the 5th place.
      var feedback = document.getElementById("chnl" + theNum);// Get the input that will have the new color.
      feedback.style.color = theNewColor;             // Change the input to the new color.
      peLS['PE_Chnl_Color'][theNum] = theNewColor;    // Update the session with the new color.
    } // End of looping through each chnl.
    colorPickerDisplayed = false;                     // The colorPicker is no longer displayed.
  }

  function updatePrgmColors() {           // The user wants to change a prgm's color.
    var prgmColorCtrls = document.getElementsByClassName("prgmColor");// Get the prgm color controls.
    for(var chnlDex = 0; chnlDex < prgmColorCtrls.length; chnlDex++) {// Loop through each chnl.
      var theNewColor = prgmColorCtrls[chnlDex].value;    // Get this prgm's color from its color control.
      var theNum = prgmColorCtrls[chnlDex].name.substr(9);// The number starts in the 9th place.
      var feedback = document.getElementById("prgm" + theNum);// Get the input that will have the new color.
      feedback.style.color = theNewColor;                 // Change the input to the new color.
      peLS['PE_Prgm_Color'][theNum] = theNewColor;    // Update the session with the new program color.
    } // End of looping through each prgm.
    colorPickerDisplayed = false;                         // The colorPicker is no longer displayed.
  }

  function changeStartHour(hour) {      // The start hour has changed.
    theStartHour = hour.toString();     // Set the start hour to the new selection.
    document.getElementById("submitBtn").click();// Refresh the page with the new start hour.
  }

  function settingsChanged() {        // The user has done something to change the settings.
    document.getElementById("submitBtn").click();// Refresh the page with the new settings.
  }

  function makeISO8601(aTime) {       // If the given time is not in ISO 8601 format return one that is.
    if(!aTime) {                      // Was a string given?
      return createNewISO8601Time();  // No, return a new ISO 8601 time.
    } // End of condition: There is not a time string.
    if(aTime.substr(4, 1) != '-') {   // Is the 5th character a '-' (date separator)?
      return createNewISO8601Time();  // No, return a new ISO 8601 time.
    } // End of condition: There is not a '-' between the year and the month.
    if(aTime.substr(7, 1) != '-') {   // Is the 8th character a '-' (date separator)?
      return createNewISO8601Time();  // No, return a new ISO 8601 time.
    } // End of condition: There is not a '-' between the month and the day.
    if(aTime.substr(10, 1) != ' ') {  // Is the 11th character a ' ' (date/time separator)?
      return createNewISO8601Time();  // No, return a new ISO 8601 time.
    } // End of condition: There is not a ' ' between the day and time of day.
    if(aTime.substr(13, 1) != ':') {  // Is the 14th character a ':' (hour/minute separator)?
      return createNewISO8601Time();  // No, return a new ISO 8601 time.
    } // End of condition: There is not a ':' between the hour and minute.
    if(aTime.length != 16) {          // Is the given time the right length?
      return createNewISO8601Time();  // No, return a new ISO 8601 time.
    } // End of condition: The given time is not the correct length.
    var n = Number(aTime.substr(0, 4));// Get the year.
    if(n = NaN || n < 1980) {         // Is the year a number greater than 1980?
      return createNewISO8601Time();  // No, return a new ISO 8601 time.
    } // End of condition: The year is illegal.
    n = Number(aTime.substr(5, 2));   // Get the month.
    if(n = NaN || n < 1 || n > 12) {  // Is the month a number between one and twelve?
      return createNewISO8601Time();  // No, return a new ISO 8601 time.
    } // End of condition: The month is illegal.
    n = Number(aTime.substr(8, 2));   // Get the day.
    if(n = NaN || n < 1 || n > 31) {  // Is the month a number between one and thirty-one?
                                      // This is not perfect because some months have fewer than 31 days.
      return createNewISO8601Time();  // No, return a new ISO 8601 time.
    } // End of condition: The day is illegal.
    n = Number(aTime.substr(11, 2));  // Get the hour.
    if(n = NaN || n < 0 || n > 23) {  // Is the hour a number between zero and twenty-three?
      return createNewISO8601Time();  // No, return a new ISO 8601 time.
    } // End of condition: The hour is illegal.
    n = Number(aTime.substr(14, 2));  // Get the minute.
    if(n = NaN || n < 0 || n > 59) {  // Is the hour a number between zero and fifty-nine?
      return createNewISO8601Time();  // No, return a new ISO 8601 time.
    } // End of condition: The minute is illegal.
    return aTime;                     // The given time was in the correct format.
  } // End of function makeISO8601.

  function createNewISO8601Time() {     // Create a string containing the time now in ISO 8601 format.
    var now = new Date();               // Create now as a new time.
    // Unfortunately, now is not in ISO 8601 format. So, it needs to be converted to ISO 8601,
    var nowTime = now.getFullYear().toString() + '-';// Start the conversion with the year & a dash.
    var month = (now.getMonth() + 1).toString();// Get the month.
    if(month.length < 2) {              // Does the month have two characters?
      month = "0" + month;              // No. It is a single digit month. Prepend a 0.
    } // End of condition: The month only has one charcter.
    nowTime += month + "-";             // Append the month and a dash.
    var day = now.getDate().toString(); // Get the day.
    if(day.length < 2) {                // Does the day have two chactacters?
      day = "0" + day;                  // No. It is a single digit day. Prepend a 0.
    } // End of condition: The day only has one charcter.
    nowTime += day + " ";               // Append the day and a space to the time.
    var hour = now.getHours().toString();// Get the hours.
    if(hour.length < 2) {               // Do the hours have two chactacters?
      hour = "0" + hour;                // No. It is a single digit Hour. Prepend a 0.
    } // End of condition: The hour only has one charcter.
    nowTime += hour + ":00";            // The minutes are always zero.
    return nowTime;                     // Return the ISO 8601 time for now.
  } // End of function createNewISO8601Time.

  // Add an event to respond to the user clicking the submit button.
  document.getElementById("tvtConfigForm").addEventListener("submit", submitClicked);

  function submitClicked() {          // The form is being submitted.
    for(var dex = 0; dex < theChnls.length; dex++) {// Loop through each chnl.
      var chnlPlot = 0;
      if(document.getElementById("chkbx" + dex).checked === true) {// Is this chnl plotted.
        chnlPlot = 1;                       // Yes. Set the chnl's plot flag to 1.
      } // End of condition: This chnl is plotted.
      peLS['PE_Chnl_Plot'][dex] = chnlPlot; // Save the session chnl plot flag.
      var prgmPlot = 0;                     // By default the program is not plotted.
      if(document.getElementById("prgmChkbx" + dex).checked === true) {
        // Is the checkbox next to this channels program checked?
        prgmPlot = 1;                       // Yes. 1 indicates the program should be plotted.
      } // End of condition: The program should be plotted.
      peLS['PE_Prgm_Plot'][dex] = prgmPlot; // Save the session prgm plot flag.
      var chnlColorValue = document.getElementById("color" + dex).value;// Get this chnl's color from its control.
      peLS['PE_Chnl_Color'][dex] = chnlColorValue;// Save this chnl's session color.
      var prgmColorValue = document.getElementById("prgmColor" + dex).value;// Get this prgm's color from its control.
      peLS['PE_Prgm_Color'][dex] = prgmColorValue;// Save this chnl's PRGM session color.
      if(peLS['isUsed'] == 'true') {        // Does this user allow local storage?
        // Yes. Store this chnl's config on the user's device.
        localStorage.setItem('PE_Chnl' + dex.toString() + '_Plot', chnlPlot);// Store the chnl plot flag.
        localStorage.setItem('PE_Prgm' + dex.toString() + '_Plot', prgmPlot);// Store the prgm plot flag.
        localStorage.setItem('PE_Chnl' + dex.toString() + '_Color', chnlColorValue);
        localStorage.setItem('PE_Prgm' + dex.toString() + '_Color', prgmColorValue);
      } // End of condition: This user allows local storage.
    } // End of looping through each channel.
    var jTest = JSON.stringify(theChnls);
    document.getElementById("theChnls").value = jTest;
    var sd = document.getElementById("startDate").value;
    // Re-format the start date to use the database's format.
    var theStartDate = sd.substr(6, 4) + "-";// Put the year first.
    theStartDate += sd.substr(0, 2) + "-";  // Append the month and the dash after it.
    theStartDate += sd.substr(3, 2);  // Append the day.
    var theStartTime = theStartDate + " ";
    if(theStartHour.length < 2) {
      theStartHour = "0" + theStartHour;
    }
    theStartTime += theStartHour + ":00";
    peLS['PE_TVT_StartTime'] = theStartTime;
    peLS['PE_TVT_Max'] = document.getElementById("setMax").value;// Get the graph's highest coordinate.
    peLS['PE_TVT_Min'] = document.getElementById("setMin").value;// Get the graph's lowest coordinate.
    if(document.getElementById("degreesF").checked == true) {// Are degrees F selected for units?
      peLS['PE_TVT_Units'] = "F";           // Set the units to Fahrenheit.
    } else {                                // Degrees Celsius were selected.
      peLS['PE_TVT_Units'] = "C";           // Set the units to Celsius.
    } // End of condition: Which units are selected.
    if(peLS['isUsed'] == 'true') {      // Does this user allow local storage?
      localStorage.setItem('PE_TVT_StartTime', theStartTime);// Save the start time on this user's device.
      localStorage.setItem('PE_TVT_Span', peLS['PE_TVT_Span']);// Save the span on this user's device.
      localStorage.setItem('PE_TVT_Max', peLS['PE_TVT_Max']);// Store the graph's highest coordinate.
      localStorage.setItem('PE_TVT_Min', peLS['PE_TVT_Min']);// Store the graph's lowest coordinate.
      localStorage.setItem('PE_TVT_Units', peLS['PE_TVT_Units']);// Store the graph's temperture units.
    }
    document.getElementById("peLS").value = JSON.stringify(peLS);// Pass the session config variable back.
} // End of function submitClicked.

  // When the user clicks the settings button, opens the settings modal dialog.
  sBtn = document.getElementById("settingsBtn").onclick = function() {
    document.getElementById("settingsModal").style.display = "block";// Display the settings dialog.
  }

  function shiftBy(amount) {                  // Shift the start hour.
    var tmpMoment = new moment(peLS['PE_TVT_StartTime']); // Create a moment in order to do time addtion.
    tmpMoment.add(amount, 'hour');            // Adjust the time by the number of hours provided by the user.
    peLS['PE_TVT_StartTime'] = tmpMoment.format('YYYY-MM-DD HH:mm');// Transfer the moment back to an ISO 8601 time.
    var theStartDate = peLS['PE_TVT_StartTime'].substr(0, 10);// The first 10 characters of the start time are the date.
    var sdElement = document.getElementById("startDate");// Access the start date element.
    // Create a string containg the date in US format.
    var usDate = peLS['PE_TVT_StartTime'].substr(5, 2) + "-";// Start with the month.
    usDate += peLS['PE_TVT_StartTime'].substr(8, 2) + "-";// Then the day.
    usDate += peLS['PE_TVT_StartTime'].substr(0, 4);// Finally, the year.
    sdElement.value = usDate;                 // Set the start date element to the new date.
    theStartHour = peLS['PE_TVT_StartTime'].substr(11, 2);// The 11th and 12th characters contain the hour.
    if(theStartHour.substr(0,1) == "0") {     // Does the hour start with a zero?
      theStartHour = theStartHour.substr(1, 1);// Yes. The zero must be removed for the startHour element.
    } // End of condition: The start hour began with a zero.
    document.getElementById("submitBtn").click();         // Refresh the page with the new start hour.
  }

  function changeSpan(amount) {                   // The graph's time span has changed.
    peLS['PE_TVT_Span'] = amount;                 // Set the new time span.
    document.getElementById("submitBtn").click(); // Refresh the page with the new time span.
  }

  document.getElementById("chnlHeader").addEventListener("click", showChnls);

  document.getElementById("prgmHeader").addEventListener("click", showPrgms);

  function showChnls(){                   // The user clicked on the header of the chnl list.
    var chnlList = document.getElementById("chnlSelect");// Create a variable to access the list of chnls.
    var theStyle = chnlList.style.display;// Get the display state of the list.
    if(theStyle == "none") {              // Is the list currently hidden?
      chnlList.style.display = "block";   // Yes. Display the chnl list.
    } else {                              // The chnl list is currently displayed.
      chnlList.style.display = "none";    // Hide the chnl list.
    } // End of condition: What is the display state of the chnl list.
  }

  function showPrgms(){                   // The user clicked on the header of the prgm list.
    var prgmList = document.getElementById("prgmSelect");   // Create a variable to access the list of chnls.
    var theStyle = prgmList.style.display;// Create a variable to access the list of prgms.
    if(theStyle == "none") {              // Is the list currently hidden?
      prgmList.style.display = "block";   // Yes. Display the prgm list.
    } else {                              // The prgm list is currently displayed.
      prgmList.style.display = "none";    // Hide the prgm list.
    } // End of condition: What is the display state of the prgm list.
  }

  document.getElementById("warningStorageOKBtn").onclick = function() {
    // The user is OK with using local storage in spite of the warning.
    peLS['isUsed'] = 'true';            // This user is going to use local storage.
    loadFromLocalStorage();             // Create all the local storage space with defaults.
    document.getElementById("submitBtn").click();// Refresh the page with local storage.
  } // End of function to allow this user to use local storage.

  document.getElementById("warningStorageCancelBtn").onclick = function() {// The user doesn't want local storage.
    peLS['isUsed'] = 'false';           // Local storage is not to be used.
    document.getElementById("submitBtn").click();// Refresh the page without local storage.
  } // End of function to decline to use of local storage.

  function hexToHsl(hex) {
    if(hex.charAt(0) == "#") {  // Is the first chartacter #? # just indicates hex, it is not a part of the hex number.
        hex = hex.substr(1, 6); // Yes. Remove the # from the hex string.
    }                           // The hex string is in the correct format to process.
    var r = "0x" + hex.substr(0, 2);  //Convert the first two hex characters to a hex number for red.
    var g = "0x" + hex.substr(2, 2);  //Convert the next two hex characters to a hex number for green.
    var b = "0x" + hex.substr(4, 2);  //Convert the next two hex characters to a hex number for blue.

    var r2 = parseInt(r, 16);   // Convert the red hex number into a red integer.
    var g2 = parseInt(g, 16);   // Convert the green hex number into a red integer.
    var b2 = parseInt(b, 16);   // Convert the blue hex number into a red integer.

    r2 /= 255;   //Convert the red value to anumber between 0 and 1 (currently is 0 and 255).
    g2 /= 255;   //Convert the green value to anumber between 0 and 1 (currently is 0 and 255).
    b2 /= 255;   //Convert the blue value to anumber between 0 and 1 (currently is 0 and 255).

    var cMax = Math.max(r2, g2, b2); // Set cMax to the largest value between
    var cMin = Math.min(r2, g2, b2); // Set cMain to the smallest value between
    var delta = cMax - cMin;         // Get the difference.

    // Calculate the hue, saturation, and lightness.
    var hsl = {h:0, s:0, l:0} // Create an hsl object to return the hsl color.
    // h is the hue. The default hue is zero (red). It has units of degrees.
    // s is the saturation.The default saturation is zero (none, gray). It is a percentage between 0 and 100.
    // l is the lightness. It is a percentage between 0 and 100.
    hsl.l = (cMax + cMin) / 2;// The formula for lightness. (still not a percentage here)
    if(delta != 0) {                // If delta is zero, the color is some shade of gray (between blak and white).
        if(cMax == r2) {            // Is the color is predominately red?
            hsl.h = 60 * (((g2 - b2) / delta ) % 6);  // Yes, calculate the hue.
        } else if(cMax == g2) {     // Is the color is predominately green?
            hsl.h = 60 * (((b2 - r2) / delta ) + 2);  // Yes, calculate the hue.
        } else {                    // The color is predominately blue.
            hsl.h = 60 * (((r2 - g2) / delta ) + 4);  // Yes, calculate the hue.
        }                           // The hue has been calculated.
        hsl.s = delta / (1 - Math.abs(2 * hsl.l - 1)) * 100;// The formula for the saturation.
    }                               // Still need to convert the lightness to a percentage.
    hsl.l *= 100;                   // Convert the lightness to a percentage.

    //alert("r2 = " + r2 + ", g2 = " + g2 + ", b2 = " + b2 + "  hsl = h:" + hsl.h + " s:" + hsl.s + " l:" + hsl.l);

    var jsonHSL = JSON.stringify(hsl);  // Convert the three hsl numbers (hsl object) into a JSON string to return.
    return jsonHSL;     // Retunrs a JSON version of the hsl color.
  }

  function drawChart() {                  // Draw the time vs temperature chart.
    // Chart.js requires the HTML canvas element in order to build the chart.
    var ctx = document.getElementById("myChart").getContext('2d');// Get the canvas element.
    var hour = startMoment.get('hour');   // Get the chart start hour.
    var maxTemp = Number(peLS['PE_TVT_Max']);// Get the chart's to coordinate.
    var minTemp = Number(peLS['PE_TVT_Min']);// Get the chart's bottom coordinate.
    var month = startMoment.month() + 1;  // Get the chart's start month.
    if(month < 10) {                      // Is the month less than ten.
      month = "0" + month;                // Add a zero for display purposes.
    } // End of condition: The month only has a single digit.
    var day = startMoment.date();         // Get the chart's start day.
    if(day < 10) {                        // Is the day less than ten.
      day = "0" + day;                    // Add a zero for display purposes.
    } // End of condition: The day only has a single digit.
    var yLabel = "Temperature (F)";       // The default "Y" axis lable.
    if(peLS['PE_TVT_Units'] == "C") {     // Are the temperture units Celsius?
      yLabel = "Temperature (C)";         // Yes. Change the label for Celsius.
    } // End of conditon: The temperture units are Celsius.
    var endMoment = startMoment.clone().add(peLS['PE_TVT_Span'], 'hours');// Create the chart's end time.
    // Create the label for the chart's "X" axis.
    var xLabel = "Time starting on " + startMoment.year() + "-" + month + "-" + day + " at " + hour + ":00";
    var chnlObjs = [];                    // An array of items to plot. Includes all display settings
                                          // and the time/temperatures.
    var plotChnlDex = 0;                  // The index of the time/temperature array in "allTvtData" to plot.
                                          // "allTvtData" contains x and y coordinates (time and temperature
                                          // coordivates) for each channel that is to be plotted. Thus,
                                          // allTvtData[0] contains all the time/temperature coordinates for
                                          // the first channel to plot. This may not be the channel with
                                          // index 0. allTvtData[1] contains all the time/temperature
                                          // coordinates for the second channel to plot. etc...
    for(var nDex = 0; nDex < theChnls.length; nDex++) {// Loop through each chnl.
      if(peLS['PE_Chnl_Plot'][nDex] == 0) {// Is this chnl supposed to plot?
        continue;                         // No. Try the next chnl.
      } // End of conditon: This chnl is not supposed to be plotted.
      // Get the color to use plotting this relay.
      var colorStr = peLS['PE_Chnl_Color'][nDex];
      chnlPlotSettings = {
        label: theChnls[nDex].name,
        fill: false,
        lineTension: 0.1,
        backgroundColor: colorStr,      // Set the line fill color.
        borderColor: colorStr,          // Set the line color.
        borderCapStyle: 'butt',         // Set the line's end cap. butt=>The ends of lines
                                        // are squared off at the endpoints. round=>The ends of
                                        // lines are rounded. square=>The ends of lines are squared off by adding
                                        // a box with an equal width and half the height of the line's thickness.
        borderDash: [],                 // Length and spacing of dashes.
        // see https://developer.mozilla.org/en-US/docs/Web/API/CanvasRenderingContext2D/setLineDash
        borderDashOffset: 0.0,          // Offset for line dashes.
        // see https://developer.mozilla.org/en-US/docs/Web/API/CanvasRenderingContext2D/lineDashOffset
        borderJoinStyle: 'miter',       // Line joint style.
        // round => Rounds off the corners of a shape by filling an additional
        // sector of disc centered at the common endpoint of connected segments.
        // The radius for these rounded corners is equal to the line width.
        // bevel => Fills an additional triangular area between the common
        // endpoint of connected segments, and the separate outside rectangular
        // corners of each segment.
        // miter => Connected segments are joined by extending their outside
        // edges to connect at a single point, with the effect of filling an
        // additional lozenge-shaped area. This setting is affected by the
        // miterLimit property. Default value.
        pointBorderColor: colorStr,     // Set the border color for points.
        pointBackgroundColor: colorStr, // Set the fill color for points.
        pointBorderWidth: 1,            // Set the width of the point border in pixels.
        pointHoverRadius: 5,            // Set the radius of the point when hovered.
        pointHHoverBackgroundColor: "rgba(75,192,192,1)",// Set the Point background color when hovered.
        pointHoverBorderColor: "rgba(220,220,220,1)",// Set the Point background color when hovered.
        pointHoverBorderWidth: 2,       // Set the Border width of point when hovered.
        pointRadius: 1,                 // Set the The radius of the point shape.
                                        // If set to 0, the point is not rendered.
        pointHitRadius: 10,             // Set the pixel size of the non-displayed
        data: allTvtData[plotChnlDex],  // The program data created by the server (Page Controller).
      } // End of the settings for this chnl.
      chnlObjs.push(chnlPlotSettings);  // The array of chnl time/temperature objects to plot.
      plotChnlDex++;                   // Increment the index of the chnl time/temp array.
    } // End of looping through each system chnl.

    // Build the objects for jsChart to plot programs.
    for(var nDex = 0; nDex < theChnls.length; nDex++) {
      if(peLS['PE_Prgm_Plot'][nDex] == 0) {// Is this program supposed to plot?
        continue;                         // No. Try the next program.
      } // End of conditon: This program is not supposed to be plotted.
      var colorStr = peLS['PE_Prgm_Color'][nDex];
      prgPlotSettings = {
        label: "P-" + theChnls[nDex].name,
        fill: false,
        lineTension: 0.0,
        backgroundColor: colorStr,
        borderColor: colorStr,
        borderCapStyle: 'butt',
        borderDash: [],
        borderDashOffset: 0.0,
        borderJoinStyle: 'miter',
        pointBorderColor: colorStr,
        pointBackgroundColor: colorStr,
        pointBorderWidth: 1,
        pointHoverRadius: 5,
        pointHHoverBackgroundColor: "rgba(75,192,192,1)",
        pointHoverBorderColor: "rgba(220,220,220,1)",
        pointHoverBorderWidth: 2,
        pointRadius: 1,
        pointHitRadius: 10,
        data: allTvtData[plotChnlDex],
      }
      chnlObjs.push(prgPlotSettings);
      plotChnlDex++;
    }

    var myChart = new Chart(ctx, {      // Create the chart.
        type: 'line',                   // The chart is a line chart.
        data: {                         // The data to plot.
          datasets:                     // The datasets to plot.
            chnlObjs                    // The objects previously created.
        },                              // The end of the data to plot.
        options: {                      // The options for creating the chart.
          animation: {                  // Disable animation. Animation is a problem
            duration: 0                 // if the plot is for a long time period.
          },
          maintainAspectRatio: true,    // boolean true Maintain the original canvas aspect
                                        // ratio (width / height) when resizing.
          scales: {                     // see https://www.chartjs.org/docs/latest/charts/line.html?q=scales
            xAxes: [{
  						type: "time",             // time => Display Formats The following display formats are used to
                                        // configure how different time units are formed into strings for the
                                        // axis tick marks.
  						display: true,            // Display vertical grid lines.
  						scaleLabel: {
  							display: true,          // Display the x axis label.
  							labelString: xLabel,    // Set the x axis label.
  						},
              time: {
                min: startMoment,       // Set the left most time.
                max: endMoment          // Set the right most time.
              }
  					}],
              yAxes: [{
                scaleLabel: {
    							display: true,        // Display the Y axis label.
    							labelString: yLabel,  // Set the Y axis label.
    						},
                ticks: {
                    max: maxTemp,
                    min: minTemp,
                }
              }]
          }
        }

    });
  }

  window.onload = configElements;// Configure this page's elements with the values stored on this user's device.

  window.onclick = function(event) {    // The user clicked somewhere.
    // Was the click in the chnl or prgm list?
    if(event.srcElement.id == "chnlSelect" ||   // Was it the chnl list?
      event.srcElement.id == "prgmSelect" ||    // Was it the prgm list?
      event.srcElement.id.substr(0, 5) == "color" ||  // Was it a chnl color input?
      event.srcElement.id.substr(0, 9) == "prgmColor") {// Was it a prgm color input?
      return;                                   // Yes. Nothing should be changed.
    } // End of condition: The user clicked on the chnl or prgm list.
    document.getElementById("chnlSelect").style.display = "none";// Hide the chnl list.
    document.getElementById("prgmSelect").style.display = "none";// Hide the prgm list.
    colorPickerDisplayed = false;               // The colorPicker is not being displayed.
  }

</script>


@endsection
