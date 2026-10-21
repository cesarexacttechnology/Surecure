@extends('master')
@section('title', 'Plot Time Vs Temp')

@section('content')

<head>

  <!-- http-equiv="refresh" content="1800;url=/plotRelays" causes the page to be
      refreshed every 1800 seconds (30 minutes) and to use a GET instead of a POST. -->
  <meta charset="UTF-8" http-equiv="refresh" content="1800;url=/plotRelays">

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
        margin-top: 4px;
        background-color: rgba(0,0,0,0.001);
    }

    .dropdown-content {
      display: none;
      position: absolute;
      min-width: 160px;
      box-shadow: 0px 8px 16px 0px rgba(0,0,0,0.2);
      border-style: solid;
      border-width: thin;
      border-color: gray;
      border-radius: 4px;
      padding: 12px 16px;
      z-index: 1;
    }
/*
    .dropdown:hover .dropdown-content {
        display: block;
    }
*/
    #relayHeader, #submitBtn {
      border-style: solid;
      border-width: thin;
      border-color: gray;
      border-radius: 4px;
      padding: 2px 4px;
      background-color: #f1f1f1;
    }

    #startDate {
      width: 85px;
      text-align: center;
      border-style: solid;
      border-width: thin;
      border-color: gray;
      border-radius: 4px;
      background-color: #f1f1f1;
      padding: 2px 4px;
      margin-top: 4px;
    }

    #startDate:hover {background-color: #56e35c;}

    #startTimeData {
      margin: 0px 5px;
      float:left;
    }

    #relaySelect {
      display: none;
      height: 200px; overflow: auto; width: 100px; border: 1px solid #000;
      list-style-type: none; margin: 0; padding: 0; overflow-x: hidden;
      margin: 0px 2px;
    }

    #relayColor {
      margin: 0px 5px;
      float:left;
      display: none;
    }

    label { display: block; color: WindowText; background-color: Window; margin: 0; padding: 0px 0px 0px 2px; width: 100%; }
    label:hover { background-color: #CBF6D2; color: HighlightText; }

    #submitBtn:hover {background-color: #56e35c;}

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
<!-- Create the form which holds the configuration for plotting time vs temperature. -->
<form id="rhConfigForm" method="post">

  <!-- Display any errors. -->
  @foreach ($scErrors as $error)
    <p class="alert alert-danger">{{ $error }}</p>
  @endforeach

  <!-- The next line submits the token required by Laravel -->
  <input type="hidden" name="_token" value="{!! csrf_token() !!}">
  <!-- Create a hidden input to hold the channel configurations for this system. -->
  <input type="hidden" id="theRelays" name="theRelays">
  <!-- Create a hidden input to hold the session configuration. -->
  <input type="hidden" id="peLS" name="peLS">

  <div>
      <!-- Configures the controls used to select the starting date. -->
  		<div id="startTimeData">
  			Start Date: <!-- Create the start date control. -->
        <input id="startDate" type="text">
        <!-- The name is important so that the start date can be passed to and from the controller. -->
        <!-- <input id="startDate" type="date" name="startDate"> -->
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
        <!-- Create the "Span:" drop down menu to alter the time plotted. -->
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

      <div class="dropdown">  <!-- Creates the drop down control for selecting the relay to plot. -->
        <div onmouseover="displayRelays()" onmouseout="hideRelays(event)" id="relayHeader">Relays</div>
        <ul onmouseout="hideRelays(event)" class="dropdown-content" id="relaySelect"></ul>
      </div>
    	<input id="submitBtn" type="submit" value="Update">

      <!--<div id="relayColor">-->
        <div class="dropdown" id="relayColor">
        Color:
        <!-- The name is important so that the relay's color can be passed to and from the input. -->
        <input id="relayColorPicker" type="color" name="relayColorInput" value="#ff0000">
      </div>

  </div>
</form>

<div>
  <h3 style="text-align:center;">Plot Relay History</h3>
  <!-- Create a canvas for chart.js to draw the relay history. -->
	<canvas id="myChart" width=400px height=200px></canvas>
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

<!-- moment is required by chart.js -->
<script src="js/moment.min.js"></script>
<script src="js/Chart.bundle.min.js"></script>

<script>
  relays = <?php echo json_encode($relays); ?>; // Put the relay history configuration in a json string.
  rhData = <?php echo json_encode($rhData); ?>; // Put the relay history data into a json string.
  peLS = <?php echo json_encode($peLS); ?>      // Put the session configuration in a json string.
  theStartHour = peLS['PE_Relay_StartTime'].substr(11, 2);// The 11th and 12th characters contain the hour.
  theStartTime = "";                            // Create a global start time.
  colorPickerDisplayed = false;                 // The colorPicker has not been displayed yet.

  function displayRelays() {
    var relayList = document.getElementById("relaySelect");// Get the relay list.
    relayList.style.display = "block";  // Display the relay list.
    relayList.style.position = "absolute";
    document.getElementById("relayHeader").style.backgroundColor = "#56e35c";
  }

  function hideRelays(event) {
    // Set the background color to the value it has when relays are not being selected.
    document.getElementById("relayHeader").style.backgroundColor = "#f1f1f1";
    var fromIdOK = false;             // Only respond if the event is from an id with
                                      // color, relay, or chkbx in the id.
    // Are the fromElement and relatedTarget elements defined.
    if(event.fromElement == null || event.relatedTarget == null) {
      return;                         // No. there is no way to respond.
    } // End of condition: fromElement and relatedTarget don't exist.
    if(event.fromElement.id.substr(0, 5) == "color") {// Is this event from a color picker?
      fromIdOK = true;                // Yes. It is OK to hide the relay list.
    } else if(event.fromElement.id.substr(0, 5) == "relay") {// Is this event from a relay element?
      fromIdOK = true;                // Yes. It is OK to hide the relay list.
    } else if(event.fromElement.id.substr(0, 5) == "chkbx") {// Is this event from a relay checkbox?
      fromIdOK = true;                // Yes. It is OK to hide the relay list.
    } // End of conditions: The event is from an element that requires hiding the relays.
    if(fromIdOK == false) {           // Should this event be ignored?
      return;                         // Yes.
    } // End of condition: This event should be ignored.
    if(colorPickerDisplayed == true) {// Is the colorPicker being displayed?
      return;                         // Yes. Don't hide the relay list.
    } // End of condition: The colorPicker is displayed.
    if(event.relatedTarget.id.substr(0, 5) == "color") {// Is the next element a colorPicker?
      return;                         // Yes. Don't hide the relay list.
    } else if(event.relatedTarget.id.substr(0, 5) == "relay") {// Is the next element a relay?
      return;                         // Yes. Don't hide the relay list.
    } else if(event.relatedTarget.id.substr(0, 5) == "chkbx") {// Is the next element a relay checkbox?
      return;                         // Yes. Don't hide the relay list.
    }// End of conditions: The relay list should still be displayed
    // The relay list should be hidden.
    document.getElementById("relaySelect").style.display = "none";  // Hide the relay list.
  } // End of function hideRelays.

  function configElements() {         // The page is being drawn. Configure the pages elements.
    if(peLS['isUsed'] == 'unknown' || peLS['isUsed'] == null) {// Has the use of local storage been determined?
      checkLocalStorage();            // Unknown. Check to see if it is used. If it is, load the config.
      if(peLS['isUsed'] == 'unknown') { // Condition: Local storage been evaluated. Is local storage unknow?
        // Yes. Use the warningDlg to ask the user if they would like to use local storage.
        var warningDlg = document.getElementById("warningStorageModal");
        warningDlg.style.display = "block"; // Display the warning dialog.
      } // End of condition: Unknown usage of local storage.
    } // End of condition: The use of local storage is unknown.
    theStartTime = peLS['PE_Relay_StartTime'];// Get the start time on this user's device.
    theStartTime = makeISO8601(theStartTime);// Make sure the start time is in ISO 8601 format.
    // Create a string containg the date in US format.
    var usDate = theStartTime.substr(5, 2) + "-" + theStartTime.substr(8, 2) + "-" + theStartTime.substr(0, 4);
    var theStartDate = theStartTime.substr(0, 10);// The first 10 characters of the start time are the date.
    var sdElement = document.getElementById("startDate");// Access the start date element.
    sdElement.value = usDate;             // Set the start date to the locally stored value.
    jQuery('#startDate').datetimepicker({ // Configure the datetimepicker.
      //  format:'d.m.Y H:i',
      format:'m-d-Y',                     // Set the display format to a US date with no time.
      //  inline:true,
      timepicker:false,                   // Don't show the timepicker.
      inline:false,                       // It is not to come up in drop down format.
      startDate:theStartDate,             // Set the date using ISO 8601 YYYY-MM-DD.
      onChangeDateTime:settingsChanged,   // Call an event handler if the user changes the date.
    }); // The datetimepicker is configured,
    theStartHour = theStartTime.substr(11, 2);  // The 11th and 12th characters contain the hour.
    if(theStartHour.substr(0,1) == "0") {       // Does the hour start with a zero?
      theStartHour = theStartHour.substr(1, 1); // Yes. The zero must be removed for the startHour element to work.
    }                                           // The start hour is in the right format.
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
    var s = "Span: ";                           // The span drop down starts with the word Span:.
    var a = peLS['PE_Relay_Span'] + " hours";     // The default after Span: is the number of hours to plot.
    if(peLS['PE_Relay_Span'] > 192) {             // Are the number of hours 1.14 weeks?
      a = peLS['PE_Relay_Span'] / 168 + " weeks"; // Yes. Humans prefer the number of weeks verses hours or days.
    } else if (peLS['PE_Relay_Span'] >= 24) {     // Are the number of hours at least 24?
      a = peLS['PE_Relay_Span'] / 24 + " days";   // Use days instead of hours.
    } // End of condition: at least 24 hours are being plotted.
    document.getElementById("span").innerHTML = s + a;// Set the Span controls text.
    document.getElementById("startHour").innerHTML = "Start Hour: " + theStartHour;
    var relayList = document.getElementById("relaySelect");// Create a variable to access the list of relays.
    var i = 0;                                  // Start with the first relay.
    for(; i < relays.length; i++) {             // Go through all the relays.
      var relayItem = document.createElement("li");// Create a list item for this relay.
      // Create a text node containing this relay's number (not index).
      var called = document.createTextNode((i + 1).toString());
      var myLabel = document.createElement("label");// Create a label for this relay.
      myLabel.setAttribute("id", "relay" + i);  // Set the id for this relay's label.
      // Get this session's hex version of this relay's color.
      myLabel.style.color = peLS['PE_Relay_Color'][i];// Set the label's color to the relay's color.
      var myInput = document.createElement("input");// Create an input for this relay.
      myInput.setAttribute("type", "checkbox"); // Make this relay's input a check box.
      myInput.setAttribute("name", "chkbx" + i);// Give this relay's check box a name.
      myInput.setAttribute("id", "chkbx" + i);  // Give this relay's check box an id.
      // The plot flag is 1 when a relay is to be plotted (0 means don't plot).
      relays[i].plot = peLS['PE_Relay_Plot'][i];// Set this relay's plot flag.
      if(relays[i].plot == 1) {                 // Is this relay supposed to be plotted?
        myInput.setAttribute("checked", "true");// Yes. Check its check box.
      } // End of condition: This relay is supposed to be plotted.
      var myInput2 = document.createElement("input");// Create an input for changing the color.
      myInput2.setAttribute("type", "color");   // The input is for changing this relay's color.
      myInput2.setAttribute("name", "color" + i);// Give this input an name.
      myInput2.setAttribute("id", "color" + i); // Give this input an id.
      myInput2.setAttribute("value", peLS['PE_Relay_Color'][i]);// Set the color.
      myInput2.setAttribute("class", "relayColorClass");// Give the color input a class.
      myInput2.addEventListener("input", updateColors);// Add a function to handle changes in the color.
      myInput2.addEventListener("click", displayColorPicker);
      // The following line makes Chrome work better (drop down disappears when the mouse leaves).
      // But, it prevents the user from selecting some colors in Safari.
      //myInput2.addEventListener("mouseout", function() { hideColorPicker(event); } );
      myLabel.appendChild(myInput);             // Add the check box input to the label.
      myLabel.appendChild(myInput2);            // Add the color in[ut to the label.
      myLabel.appendChild(called);              // Add text for the label to display.
      relayItem.appendChild(myLabel);           // Add the label to the list item.
      relayList.appendChild(relayItem);         // Add the list item to the list.
    } // End of looping through all the relays.
    relayList.addEventListener("change", relaysChanged);// Respond to a change in relay configurations.
    drawChart();                                // Draw the percetage-on chart.
  } // End of function configElements.

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
  }

  function createNewISO8601Time() {
    var now = new Date();                 // Create now as a new time.
    // Unfortunately, now is not in ISO 8601 format. So, it needs to be converted to ISO 8601,
    var nowTime = now.getFullYear().toString() + '-';// Start the conversion with the year & a dash.
    var month = (now.getMonth() + 1).toString();// Get the month.
    if(month.length < 2) {                // Does the month have two characters?
      month = "0" + month;                // No. It is a single digit month. Prepend a 0.
    } // End of condition: The month only has one charcter.
    nowTime += month + "-";               // Append the month and a dash.
    var day = now.getDate().toString();   // Get the day.
    if(day.length < 2) {                  // Does the day have two chactacters?
      day = "0" + day;                    // No. It is a single digit day. Prepend a 0.
    } // End of condition: The day only has one charcter.
    nowTime += day + " ";                 // Append the day and a space to the time.
    var hour = now.getHours().toString(); // Get the hours.
    if(hour.length < 2) {                 // Do the hours have two chactacters?
      hour = "0" + hour;                  // No. It is a single digit Hour. Prepend a 0.
    } // End of condition: The hour only has one charcter.
    nowTime += hour + ":00";              // The minutes are always zero.
    return nowTime;                       // Return the ISO 8601 time for now.
  }


/*
  // This function could be deleted in the future. It was used when the colors were stored in the
  // database and not on the user's device.
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
  } // End of function hslToHex.
*/

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

  function displayColorPicker() {
    colorPickerDisplayed = true;
  }

  function hideColorPicker(event) {
    colorPickerDisplayed = false;
  }

  function updateColors() {         // The user wants to change a relay's color.
    var colorCtrls = document.getElementsByClassName("relayColorClass");// Get the color controls.
    for(var rDex = 0; rDex < colorCtrls.length; rDex++) {// Loop through each relay.
        var theNewColor = colorCtrls[rDex].value;     // Get this relay's color from its color control.
        var theNum = colorCtrls[rDex].name.substr(5); //
        var feedback = document.getElementById("relay" + theNum);// Get the input that will have the new color.
        feedback.style.color = theNewColor;           // Change the input to the new color.
//        localStorage.setItem('PE_Relay' + theNum + '_Color', theNewColor);// Store the new color on the user's device.
        peLS['PE_Relay_Color'][theNum] = theNewColor; // Store the new color on the user's device.
    } // End of looping through the relays.
    colorPickerDisplayed = false;                     // The colorPicker is no longer displayed.
  }

  function showRelays() {                 // The user clicked on the header of the relay list.
    var relayList = document.getElementById("relaySelect");// Get the relay list.
    var theStyle = relayList.style.display;// Get the display state of the list.
    if(theStyle == "none") {              // Is the list currently hidden?
      relayList.style.display = "block";  // Yes. Display the relay list.
    } else {                              // The relay list is currently displayed.
      relayList.style.display = "none";   // Hide the relay list.
    } // End of condition: What is the display state of the relay list.
  }

  function settingsChanged() {        // The user has done something to change the settings.
    document.getElementById("submitBtn").click();// Refresh the page with the new start hour.
  }

  function relaysChanged() {        // The user has done something to change the relays.
    for(var dex = 0; dex < relays.length; dex++) {// Go through each relay.
      if(document.getElementById("chkbx" + dex).checked === true) {// Is this relay supposed to plot?
        relays[dex].plot = 1;         // Yes. Set the relay's plot flag.
      } else {                        // No. This relay is not supposed to plot.
        relays[dex].plot = 0;         // Clear the relay's plot flag.
      } // End of condition: Is this relay suppose to plot.
      peLS['PE_Relay_Plot'][dex] = relays[dex].plot;// Store this relay's plot flag in the session config.
      // The HTML color element stores the color as a hex number.
      var colorValue = document.getElementById("color" + dex).value;
      peLS['PE_Relay_Color'][dex] = colorValue;// Store this relay's color in the session config.
      if(peLS['isUsed'] == 'true') {  // Does this user allow local storage?
        // Yes. Store this relay's plot flag.
        localStorage.setItem('PE_Relay' + dex.toString() + '_Plot', relays[dex].plot);
        // Store this relay's color.
        localStorage.setItem('PE_Relay' + dex.toString() + '_Color', colorValue);
      } // End of condition: This user allows local storage.
    } // End of looping through all the relays.
    document.getElementById("submitBtn").click();// Refresh the page with the new relay settings.
  }

  document.getElementById("relayHeader").addEventListener("click", showRelays);// Handle a change in relays.

  // Add an event to respond to the user clicking the submit button.
  document.getElementById("rhConfigForm").addEventListener("submit", submitClicked);

  function shiftBy(amount) {                  // The users is changing the start time.
    var tmpMoment = new moment(theStartTime); // Create a moment in order to do time addtion.
    tmpMoment.add(amount, 'hour');            // Adjust the time by the number of hours provided by the user.
    theStartTime = tmpMoment.format('YYYY-MM-DD HH:mm');// Transfer the moment back to an ISO 8601 time.
    var theStartDate = theStartTime.substr(0, 10);// The first 10 characters of the start time are the date.
    var sdElement = document.getElementById("startDate");// Access the start date element.
    // Create a string containg the date in US format.
    var usDate = theStartTime.substr(5, 2) + "-" + theStartTime.substr(8, 2) + "-" + theStartTime.substr(0, 4);
    sdElement.value = usDate;                 // Set the start date element to the new date.
    theStartHour = theStartTime.substr(11, 2);// The 11th and 12th characters contain the hour.
    if(theStartHour.substr(0,1) == "0") {     // Does the hour start with a zero?
      theStartHour = theStartHour.substr(1, 1);// Yes. The zero must be removed for the startHour element.
    } // End of condition: The start hour began with a zero.
    document.getElementById("submitBtn").click();// Refresh the page with the new start time.
  }

  function changeStartHour(hour) {                // The start hour has changed.
    theStartHour = hour.toString();               // Set the start hour to the new selection.
    document.getElementById("submitBtn").click(); // Refresh the page with the new start hour.
  }

  function changeSpan(amount) {                   // The graph's time span has changed.
    peLS['PE_Relay_Span'] = amount;               // Set the new time span.
    document.getElementById("submitBtn").click(); // Refresh the page with the new time span.
  }

  function submitClicked() {              // Submit the configuration change to the page controller.
    var jRelays = JSON.stringify(relays); // Convert the relays to a JSON string for the Controller.
    document.getElementById("theRelays").value = jRelays;// Put the JSON string in the hidden control.
    theStartDate = document.getElementById("startDate").value;// Get the start date.
    // Create a string containg the date in ISO 8601 format.
    theStartTime = theStartDate.substr(6, 4) + "-" + theStartDate.substr(0, 2) + "-" + theStartDate.substr(3, 2);
    theStartTime += " ";                  // A space separates the date from the time of day.
    if(theStartHour.length < 2) {         // ISO 8601 requires two charcters for the hour.
      theStartTime += "0" + theStartHour; // The hour only had one charcater. Prepend a 0 to make two.
    } else {                              // The hour had two characters.
      theStartTime += theStartHour;       // Add the start hour to the ISO 8601 time string.
    }                                     // The ISO 8601 contains the start hour.
    theStartTime += ":00";                // Add the start minute which is always "00".
                                          // The graph always starts at the start of the start hour.
    peLS['PE_Relay_StartTime'] = theStartTime;// Save the relay history start time to the session config.
    if(peLS['isUsed'] == 'true') {        // Does this user allow local storage?
      // Yes. Store this chnl's config on the user's device.
      localStorage.setItem('PE_Relay_StartTime', theStartTime);// Save the start time on this user's device.
      localStorage.setItem('PE_Relay_Span', peLS['PE_Relay_Span']);// Save the span on this user's device.
    } // End of condition: This user allows local storage.
    // Convert the session configuration object into a JSON string.
    document.getElementById("peLS").value = JSON.stringify(peLS);
  } // End of function submitClicked.

/*
  // This function could be deleted in the future. It was used when the colors were stored in the
  // database and not on the user's device.
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
  } // End of function hexToHsl.
*/

  window.onload = configElements;       // Configure all the configuration elements when the page loads.

  window.onclick = function(event) {    // The user clicked somewhere.
    // Was the click in the relay list?
    if(event.srcElement.id == "relaySelect" || event.srcElement.id.substr(0, 5) == "color") {
      return;                           // Yes. Nothing should be changed.
    } // End of condition: The user clicked on the relay list.
    document.getElementById("relaySelect").style.display = "none";   // Hide the relay list.
  }

  function drawChart() {                // Draw the relay history chart.
    // Chart.js requires a moment object for the start time.
    var startMoment = new moment(theStartTime);
    // Chart.js requires the HTML canvas element in order to build the chart.
    var ctx = document.getElementById("myChart").getContext('2d');
    var hour = startMoment.get('hour'); // Get the start hour.
    var month = startMoment.month() + 1;// Get the start month.
    if(month < 10) {                    // Does the month have less than two digits?
      month = "0" + month;              // Yes. Prepend a zero.
    } // End of condition: The month only had one digit.
    var day = startMoment.date();       // Get the start day.
    if(day < 10) {                      // Does the day have less than two digits?
      day = "0" + day;                  // Yes. Prepend a zero.
    } // End of condition: The day only had one digit.
    var yLabel = "Percentage on";       // The Y axis contains the percentage on.
    // Chart.js uses a moment object to know how wide the chart is.
    var endMoment = startMoment.clone().add(peLS['PE_Relay_Span'], 'hours');
    // Create a label for the X axis (time axis) which includes the start time.
    var xLabel = "Time starting on " + month + "-" + day + "-"  + startMoment.year() + " at " + hour + ":00";
    var rlyObjs = [];                   // An array of items to plot. Includes all display settings and the time/percentages.
    var plotRelayDex = 0;               // The index of the relay history array in "rhData" to plot.
                                        // "rhData" contains x and y coordinates (time and percetage coordivates) for
                                        // each relay that is to be plotted. Thus, rhData[0] contains all the
                                        // time/percetage coordinates for the first relay to plot. This may not be the
                                        // relay with index 0. rhData[1] contains all the time/percenteage coordinates for
                                        // the second relay to plot. etc...
    for(var nDex = 0; nDex < relays.length; nDex++) {// Loop through each system relay.
      if(relays[nDex].plot == 0) {      // Is this relay supposed to be plotted?
        continue;                       // No. Go on to the next relay.
      } // End of condition: This relay is not supposed to be plotted.
      // Get the color to use plotting this relay.
      var colorStr = peLS['PE_Relay_Color'][nDex];
      chnlPlotSettings = {              // Create the plot settings for this relay.
        label: (nDex + 1).toString(),   // This is the label to put at the chart's top.
        fill: false,                    // Don't fill under the line.
        lineTension: 0.1,               // Set the Bezier curve tension of the line. Set to 0 to draw straightlines.
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
        pointHoverBackgroundColor: "rgba(75,192,192,1)",// Set the Point background color when hovered.
        pointHoverBorderColor: "rgba(220,220,220,1)",// Set the Point background color when hovered.
        pointHoverBorderWidth: 2,       // Set the Border width of point when hovered.
        pointRadius: 1,                 // Set the The radius of the point shape.
                                        // If set to 0, the point is not rendered.
        pointHitRadius: 10,             // Set the pixel size of the non-displayed
                                        // point that reacts to mouse events.
        data: rhData[plotRelayDex],     // The relay history data created by the page controller.
      } // End of the settings for this relay.
      rlyObjs.push(chnlPlotSettings);   // The array of relay history objects to plot.
      plotRelayDex++;                   // Increment the index of the relay history array.
    } // End of looping through each system relay.

    var myChart = new Chart(ctx, {      // Create the chart.
        type: 'line',                   // The chart is a line chart.
        data: {                         // The data to plot.
          datasets:                     // The datasets to plot.
            rlyObjs                     // The objects previously created.
        },                              // The end of the data to plot.
        options: {                      // The options for creating the chart.
          animation: {                  // Disable animation. Animation is a problem
            duration: 0                 // if the plot is for a long time period.
          },
          maintainAspectRatio: true,    // boolean true Maintain the original canvas aspect
                                        // ratio (width / height) when resizing.
            scales: {                   // see https://www.chartjs.org/docs/latest/charts/line.html?q=scales
              xAxes: [{
    						type: "time",           // time => Display Formats The following display formats are used to
                                        // configure how different time units are formed into strings for the
                                        // axis tick marks.
    						display: true,          // Display vertical grid lines.
    						scaleLabel: {
    							display: true,        // Display the x axis label.
    							labelString: xLabel,  // Set the x axis label.
    						},
                time: {
                  min: startMoment,     // Set the left most time.
                  max: endMoment        // Set the right most time.
                }
    					}],
                yAxes: [{
                  scaleLabel: {
      							display: true,      // Display the Y axis label.
      							labelString: yLabel,// Set the Y axis label.
      						},
                  ticks: {
                      max: 100,         // The top percentage (100%).
                      min: 0,           // The bottom percentage (0%).
                  }
                }]
            }
        }

    });
  }


</script>


@endsection
