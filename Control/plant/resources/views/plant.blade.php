@extends('master')
@section('title', 'Plant')

@section('content')



<!doctype html>
<html>

<head>
  <!-- http-equiv="refresh" content="20;url=/plotRelays" causes the page to be
      refreshed every 20 seconds and to use a GET instead of a POST. -->
    <meta charset="UTF-8" http-equiv="refresh" content="20;url=/plant">

<style>
    table, th, td {
        border: 2px solid black;
        border-collapse: collapse;
    }
    th, td {
        text-align: center;
        padding: 4px;
    }
	th {
		font-weight: bold;
	}

	tr:nth-child(even) {background: #CCC;}

	tr:nth-child(odd) {background: #FFF;}

	.red {color:red;}

  body {
    margin: 5px;
  }

  .flexContainer {
    display: flex;
    flex-direction: row;
  }

  .flexContainer > div {
    /*background-color: green;*/
    text-align: center;
  }

  #submitBtn {
    display: none;
    margin-left: 20px;
    margin-right: 20px;
    margin-top: 6px;
  }

  #clearLStorage {
    display: block;
    margin-top: 6px;
    margin-left: 20px;
    margin-right: 20px;
  }

  #PEPM {
    font-size: 24px;
  }

  #timeDisplay {
    margin-left: 70px;
    margin-right: 70px;
    margin-top: 8px;
  }

  #plantTable {
    position: relative;
    float: left;
  }


</style>

</head>

<body>

@foreach ($scErrors as $error)
  <p class="alert alert-danger">{{ $error }}</p>
@endforeach

<div class="flexContainer">

  <div id="PEPM">
    Plant Manager
  </div>

  <div id=timeDisplay>Time</div>

  <div>
    <form method="post">
      <input type="hidden" name="_token" value="{!! csrf_token() !!}">
      <!-- Create a hidden input to hold the action to be performed during a plant update. -->
      <input type="hidden" id="loggedOn" name="loggedOn" value="{!! $loggedOn !!}">
      <!-- Create a hidden input to hold the local storage status. -->
      <input type="hidden" id="peLS" name="peLS">
      <input id="submitBtn" type="submit" value="Log Out" onclick="submitClicked()">
    </div>
    <div>
      <!--<input id="clearLStorage" type="button" value="Clear Storage" onclick="clearLocalStorage()">-->
    </form>
  </div>

</div>

<div>
  <table id="plantTable">
    <colgroup>
      <col span="1" style="width: 14%;">
      <col span="3" style="width: 8%;">
      <col span="1" style="width: 14%;">
      <col span="4" style="width: 8%;">
    </colgroup>
    <tr>
        <th>Name</th>
        <th>Reading</th>
        <th>Setpoint</th>
        <th>Control Type</th>
        <th>Reference</th>
        <th>Time on</th>
        <th>Maturity</th>
        <th>Deviation</th>
        <th>Relay State</th>
    </tr>
  </table>
  <br>
  <br>
</div>

</body>

<script>

function configPage() {               // Configures how the page looks.
  if(document.getElementById("loggedOn").value == 'true') {// Is a user logged on?
    document.getElementById("submitBtn").style.display = "block";// Yes. Display the log out button.
  } // End of condition: A user is logged in.
  var d = new Date();                               // Get the current time.
  var n = d.toLocaleString();                       // Convert it to a local time string.
  document.getElementById("timeDisplay").innerHTML = n;// Display the current local time.
  var allChnls = <?php echo json_encode($chnls); ?>;// Put the all of the channels into a json string.
  var readings = <?php echo json_encode($readings); ?>; // Put the readings into a json string
  var peLS = <?php echo json_encode($peLS); ?>;     // Contains this session's configuration.
  for(var row = 0; row < allChnls.length; row++) {  // Go through all the chnl indexes
    var chnl_data_row = document.createElement("tr");// Create a row for this chnl's reading.
    var chnl_name = document.createElement("td");   // The first cell contains this chnl's name.
    var colorTxt = localStorage.getItem('PE_Chnl' + row.toString() + '_Color');// Get this chnl's locally stored color.
    if(!colorTxt) {                                 // Is there a locally stored color?
      colorTxt = peLS['PE_Chnl_Color'][row];        // No. Get the session color.
    } // End of condition: There is no local storage for this chnl's color.
    chnl_name.style.color = colorTxt;               // Set this chnl's name to the chnl color.
    var chnl_name_txt = document.createTextNode(allChnls[row].name);// Put the name in the cell's text.
    var chnl_value = document.createElement("td");  // The next cell contains this chnl's value (noramlly temperature).
    var chnl_value_txt = document.createTextNode(readings[row].valueF);// Put the tempearture in the cell's text.
    var chnl_setpoint = document.createElement("td");// The next cell contains this chnl's setpoint.
    var chnl_setpoint_txt = document.createTextNode(readings[row].setpointF);// Put the setpoint in the cell's text.
    var chnl_ctrlType = document.createElement("td");// The next cell contains this chnl's control type.
    var chnl_ctrlType_txt = document.createTextNode(readings[row].controlType);// Put the control type in the cell's text.
    var chnl_ref = document.createElement("td");    // The next cell contains this chnl's reference.
    var chnl_ref_txt = document.createTextNode(readings[row].reference);// Put the reference in the cell's text.
    var chnl_timeOn = document.createElement("td"); // The next cell contains this chnl's time on.
    var chnl_timeOn_txt = document.createTextNode(readings[row].timeOn);// Put the time on in the cell's text.
    var chnl_mat = document.createElement("td");    // The next cell contains this chnl's maturity.
    var chnl_mat_txt = document.createTextNode(readings[row].maturityF);// Put the maturity in the cell's text.
    var chnl_dev = document.createElement("td");    // The next cell contains this chnl's deviation.
    var chnl_dev_txt = document.createTextNode(readings[row].deviationF);// Put the deviation in the cell's text.
    var chnl_rState = document.createElement("td"); // The next cell contains this chnl's relay state.
    var chnl_rState_txt = document.createTextNode(readings[row].relayState);// Put the relay state in the cell's text.
    if(readings[row].relayState == "On") {          // Is this relay energized?
      chnl_rState.style.color = "red";              // Yes. Set its color to red.
    } else {                                        // This relay is not energized.
      chnl_rState.style.color = "blue";             // Set its color to blue.
    } // End of condition: What state is a relay in.

    chnl_name.appendChild(chnl_name_txt);   // Attach this chnl's name text to the name cell.
    chnl_data_row.appendChild(chnl_name);   // Attach the name cell to this chnl's table row.
    chnl_value.appendChild(chnl_value_txt); // Attach this chnl's value text to the value cell.
    chnl_data_row.appendChild(chnl_value);  // Attach the value cell to this chnl's table row.
    chnl_setpoint.appendChild(chnl_setpoint_txt);// Attach this chnl's setpoint text to the setpoint cell.
    chnl_data_row.appendChild(chnl_setpoint);// Attach the setpoint cell to this chnl's table row.
    chnl_ctrlType.appendChild(chnl_ctrlType_txt);// Attach this chnl's control type text to the control type cell.
    chnl_data_row.appendChild(chnl_ctrlType);// Attach the control type cell to this chnl's table row.
    chnl_ref.appendChild(chnl_ref_txt);     // Attach this chnl's reference text to the reference cell.
    chnl_data_row.appendChild(chnl_ref);    // Attach the reference cell to this chnl's table row.
    chnl_timeOn.appendChild(chnl_timeOn_txt);// Attach this chnl's time on text to the time on cell.
    chnl_data_row.appendChild(chnl_timeOn); // Attach the time on cell to this chnl's table row.
    chnl_mat.appendChild(chnl_mat_txt);     // Attach this chnl's maturity text to the maturity cell.
    chnl_data_row.appendChild(chnl_mat);    // Attach the maturity cell to this chnl's table row.
    chnl_dev.appendChild(chnl_dev_txt);     // Attach this chnl's deviation text to the deviation cell.
    chnl_data_row.appendChild(chnl_dev);    // Attach the deviation cell to this chnl's table row.
    chnl_rState.appendChild(chnl_rState_txt);// Attach this chnl's relay state text to the relay state cell.
    chnl_data_row.appendChild(chnl_rState); // Attach the relay state cell to this chnl's table row.
    plantTable.appendChild(chnl_data_row);  // Attach this chnl's row to the table.
  } // End of looping through all the channels.
} // End of function configPage.

function submitClicked() {            // The user clicked on something which requires posting data to the PagesController.
  document.getElementById("loggedOn").value = 'logout';// Tells the PageController to log the current user out.
}

function clearLocalStorage() {        // Clear all local storage.
  console.log("clearLocalStorage called.");
  localStorage.removeItem('PE_TVT_StartTime');// Remove the TVT start time.
  localStorage.removeItem('PE_TVT_Span');// Remove the TVT plot span.
  localStorage.removeItem('PE_TVT_Max');// Remove the TVT maximum temperature.
  localStorage.removeItem('PE_TVT_Min');// Remove the TVT minimum temperature.
  localStorage.removeItem('PE_TVT_Units');// Remove the TVT temperature units.
  var chnlColors = [];                // Create an array to hold the chnl colors.
  var colorDex = 0;                   // The first color index is 0.
  // Go through each color index that has a stored color.
  while(localStorage.getItem('PE_Chnl' + colorDex.toString() + '_Color') != null) {
    // There was a stored color for this index. Remove its local storage.
    localStorage.removeItem('PE_Chnl' + colorDex.toString() + '_Color');
    console.log("Removing local chnl color " + colorDex.toString());
    if(peLS['PE_Chnl_Color'] == null) {// Does this session color exist?
      chnlColors.push("#000000");     // Black is the default chnl color.
    } // End of condition: This session color doesn't exist.
    colorDex++;                       // Increment the color index.
  } // Done removing each locally stored color.
  if(peLS['PE_Chnl_Color'] == null) {// Do session colors exist?
    peLS['PE_Chnl_Color'] = chnlColors; // No. Create them.
  } // End of condition: Session colors don't exist.
  var i = 0;                          // The first chnl's index.
  // Go through each chnl's index that has a stored plot flag.
  while(localStorage.getItem('PE_Chnl' + i.toString() + '_Plot') != null) {
    // There was a stored plot flag for this index. Remove its local storage.
    localStorage.removeItem('PE_Chnl' + i.toString() + '_Plot');
    i++;                              // Increment the plot flag.
  } // Done removing each locally stored plot flag.
  var prgDex = 0;                     // The first program's index.
  // Go through each program's index that has a stored program color.
  while(localStorage.getItem('PE_Prgm' + prgDex.toString() + '_Color') != null) {
    // There was a stored program color for this index. Remove its local storage.
    localStorage.removeItem('PE_Prgm' + prgDex.toString() + '_Color');
    prgDex++;                         // Increment the program index.
  } // Done removing each locally stored program colors.
  var prgDex = 0;                     // The first program's index.
  // Go through each program's index that has a stored program plot flag.
  while(localStorage.getItem('PE_Prgm' + prgDex.toString() + '_Plot') != null) {
    // There was a stored program plot flag for this index. Remove its local storage.
    localStorage.removeItem('PE_Prgm' + prgDex.toString() + '_Plot');
    prgDex++;                         // Increment the program index.
  } // Done removing each locally stored program plot flag.
  localStorage.removeItem('PE_Relay_StartTime');// Remove the start time for plotting relay history.
  localStorage.removeItem('PE_Relay_Span');// Remove the time span for plotting relay history.
  colorDex = 0;                       // The first relay color index is 0.
  // Go through each relay color index that has a stored color.
  while(localStorage.getItem('PE_Relay' + colorDex.toString() + '_Color') != null) {
    // There was a stored relay color for this index. Remove its local storage.
    localStorage.removeItem('PE_Relay' + colorDex.toString() + '_Color');
    colorDex++;                       // Increment the relay color index.
  } // Done removing each locally stored relay color.
  i = 0;                            // The first relay's index.
  while(localStorage.getItem('PE_Relay' + i.toString() + '_Plot') != null) {
    // There was a stored relay plot flag for this index. Remove its local storage.
    localStorage.removeItem('PE_Relay' + i.toString() + '_Plot');
    i++;                              // Increment the relay index.
  } // Done removing each locally stored relay plot flag.
  peLS['isUsed'] = 'unknown';         // Local storage is no longer used.
  configPage();
} // End of function clearLocalStorage.

window.onload=configPage;             // When the page loads, configure how it looks.

</script>
</html>
@endsection
