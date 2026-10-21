@extends('master')
@section('title', 'Home')

@section('content')

<style>

/* The Modal (background) */
.PEmodal {
  display: none; /* Hidden by default */
  position: fixed; /* Stay in place */
  z-index: 1; /* Sit on top */
  padding-top: 13%; /* Location of the box */
  left: 0;
  top: 0;
  width: 100%; /* Full width */
  height: 100%; /* Full height */
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
  width: 500px;
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

.PECenter {
  display: flex;
  justify-content: center;
  text-align: center;
}

.PEBtnShow {
  display: flex;
  justify-content: center;
}

.PEBtnHide {
  display: none;
}

.PEWarning {
  border-width: 2px;
  border-radius: 6px;
  border-color: red;
}

.PEBorder {
  border-width: 1px;
  border-radius: 6px;
  border-color: black;
}

.PEDlgText {
  font-size: 22px;
  color: black;
}

.PEHomeBtns {
  margin: 30px;
  font-size: 22px;
  min-height: 60px;

}

.dropdown {
    position: relative;
    display: inline-block;
    margin: 0px 5px;
    border-style: solid;
    border-width: thin;
    border-color: gray;
    border-radius: 4px;
    padding: 1px 3px;
    /*background-color: rgba(0,0,0,0.01);*/
    min-width: 100px;
    text-align: center;
}

.dropdown-content {
  display: none;
  position: absolute;
  min-width: 150px;
  border-style: none;
  box-shadow: 0px 8px 16px 0px rgba(0,0,0,0.8);
  padding: 0px 0px;
  z-index: 1;
}

.dropdown:hover .dropdown-content {
    display: block;
}

</style>

<form id="homeForm" method="post">

  <!-- Display any errors. -->
  @foreach ($scErrors as $error)
    <p class="alert alert-danger">{{ $error }}</p>
  @endforeach

  <!-- http-equiv="refresh" content="15" causes the page to be refreshed every 15 seconds. -->
  <!-- <meta charset="UTF-8" http-equiv="refresh" content="15"> -->

  <!-- The next line submits the token required by Laravel -->
  <input type="hidden" name="_token" value="{!! csrf_token() !!}">
  <!-- Create a hidden input to hold the action to be performed during an update. -->
  <input type="hidden" id="action" name="action" value="{!! $action !!}">
  <!-- Create a hidden input to hold the stop count so the PageController can evaluate when to enable the stop button. -->
  <input type="hidden" id="stopBtnCount" name="stopBtnCount" value="{!! $stopBtnCount !!}">

  @if (session('status'))
    {{ session('status') }}
  @endif

  <h1 class="PECenter">Cure Manager</h1>
  <h3 class="PECenter">Helps Control Curing Concrete</h3>
  <h4 class="PECenter">Cure Manager version: 1.4</h4>
  <h4 class="PECenter">Plant Manager version: {!! $pmVersion !!}</h4>
  <h4 class="PECenter">The Plant Manager last updated {!! $runTime !!} ago.</h4>
  <div class="PECenter">
    <button class="PEHomeBtns" type="button" id="startBtn" name="startPM">Start Plant Manager</button>
    <button class="PEHomeBtns" type="button" id="stopBtn" name="stopPM">Stop Plant Manager</button>
    <!--
    <button class="PEHomeBtns">Start Plant Manager</button>
    <button class="PEHomeBtns">Stop Plant Manager</button>
  -->
  </div>
</form>

<!-- Modal -->
<div id="waitFor" class="PEmodal">
  <!-- Modal content-->
  <div class="PEmodal-content">
    <h2 style="text-align:center;">Talking to the Plant Manager.</h2>
    <h2 style="text-align:center;">Please Wait.</h2>
  </div>
</div>

<!-- The modal dialog to send a warning to the user. -->
<!-- Start here, call the warning dialog before starting or stopping thePM. -->
<div id="warningModal" class="PEmodal">
  <!-- modal content -->
  <div class="PEmodal-content PEWarning">
    <div class="PECenter">
      <h4 id="warningMessage">Message</h4>
    </div>
    <div class="PECenter">
      <button id="warningOKBtn" class="PEBtns">OK</button>
      <button id="warningCancelBtn" class="PEBtns">Cancel</button>
    </div>
  </div>
</div><!-- End of the dialog to get a yes or no response. -->

<script>


document.getElementById("startBtn").onclick = function() {
  document.getElementById("action").value = "Start";// Pass the 'start PM' command to the PagesController.
  var start_Btn = document.getElementById("startBtn");// Access the start PM button.
  start_Btn.disabled = true;               // Don't let the user click the start button twice.
  start_Btn.style.color = "lightgray";     // Lightgray makes the button look disabled.
  document.getElementById("homeForm").submit();// Have the PagesController process the request to start the PM.
}

document.getElementById("stopBtn").onclick = function() {
  document.getElementById("action").value = "Stop";// Pass the 'stop PM' command to the controller.
  var warningDlg = document.getElementById("warningModal");// Yes. Use the warningDlg to alert the user.
  var message = "<p>This will stop the Plant Manger! Do you wish to continue?</p>";// Create
  document.getElementById("warningMessage").innerHTML = message;// Set the reason.
  warningDlg.style.display = "block";// Display the warning dialog.
}

document.getElementById("warningOKBtn").onclick = function() {// The user is OK with the action in spite of the warning.
  var warningDlg = document.getElementById("warningModal");// Get acces to the warning dialog.
  if(document.getElementById("action").value == "Stop") {// Should the PM be stopped?
    warningDlg.style.display = "none";// Yes. Remove the warning dialog.
    var stopBtn = document.getElementById("stopBtn");// Access the stop PM button.
    stopBtn.disabled = true;               // Don't let the user click the stop button twice.
    stopBtn.style.color = "lightgray";     // Lightgray makes the button look disabled.
    document.getElementById("homeForm").submit();// Submit the request to stop the PM.
    document.getElementById("waitFor").style.display = "block";// Yes. This takes a while. Alert the user.
    return;                           // Done stopping the PM.
  } // End of condition to stop the PM.
} // End of function to accept the action which caused the warning.

document.getElementById("warningCancelBtn").onclick = function() {// Cancel the action associated with the warning.
  document.getElementById("action").value = "Cancel";// Pass the 'stop PM' command to the controller.
  document.getElementById("homeForm").submit();// Have the PagesController process the request to cancel the action.
} // End of function to accept the action which caused the warning.

function setBtns() {
  // Get the string which starts with the number of minutes since the last PM update.
  var minutesSincePMUpdate = parseInt("{!! $runTime !!}");
  var startBtn = document.getElementById("startBtn");// Access the button to start the PM.
  var stopBtn = document.getElementById("stopBtn");// Access the button to start the PM.
  var theAction = document.getElementById("action").value;
  if(document.getElementById("action").value == "UpdateStop") {
    var warningDlg = document.getElementById("warningModal");// Yes. Use the warningDlg to alert the user.
    var message = "<p>This will stop the Plant Manger! Do you wish to continue?</p>";// Create
    document.getElementById("warningMessage").innerHTML = message;// Set the reason.
    document.getElementById("action").value = "Stop";
    warningDlg.style.display = "block";// Display the warning dialog.
  }
  if(minutesSincePMUpdate >= 2) {     // Has it been at least 2 minutes since the PM updated its runTime?
    startBtn.disabled = false;        // Yes. Allow the user to start the PM.
    stopBtn.disabled = true;          // Don't allow the user to shut down the PM since it is not running.
    startBtn.style.color = "black";   // Black makes the button look enabled.
    stopBtn.style.color = "lightgray";// Lightgray makes the button look disabled.
  } else {                            // It has been less than 2 minutes since the PM updated its runTime.
    startBtn.disabled = true;         // Don't allow the user to start the PM.
    startBtn.style.color = "lightgray";// Lightgray makes the button look disabled.
    if("{!! $stopBtnCount !!}" > 3) {   // Have there been at least two updates since the PM started?
      stopBtn.disabled = false;       // Allow the user to shut down the PM.
      stopBtn.style.color = "black";  // Black makes the button look enabled.
    } else {                          // There have not been two updates since the PM started.
      stopBtn.disabled = true;        // Don't allow the user to shut down the PM. It needs time to start.
      stopBtn.style.color = "lightgray";// Lightgray makes the button look disabled.
    } // End of condition: Evaulate the number of updates and then set the state of the stop button.
  } // End of condition to set the disabled state of the button to start the PM.
} // End of function setBtns.

function updateTime() {
  document.getElementById("action").value = "Update";// Pass the 'Update' command to the controller.
  if(document.getElementById("warningModal").style.display == "block") {
    document.getElementById("action").value = "UpdateStop";// Pass the 'Update' command to the controller.
  }
  document.getElementById("homeForm").submit();
}

window.onload=setBtns;
window.setInterval(updateTime, 15000);

</script>



@endsection
