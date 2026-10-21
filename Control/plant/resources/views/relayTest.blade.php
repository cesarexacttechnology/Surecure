@extends('master')
@section('title', 'Plant')

@section('content')

<head>

  <!-- http-equiv="refresh" content="3600" causes the page to be refreshed every 3600 seconds (1 hour). -->
  <meta charset="UTF-8" http-equiv="refresh" content="3600">

<style>

  #testRelay {
    border-radius: 4px;
    border-color: gray;
    border-style: groove;
    border-width: thin;
    background-color: #f1f1f1;
    padding: 4px;
    margin-top: 2px;
    font-size: 16px;
    width: 50px;
    text-align-last:center;
  }

  .PELabel {
    font-size: 20px;
    margin-left: 5px;
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

  .PECenter {
    display: flex;
    justify-content: center;
    text-align: center;
  }

</style>

</head>

<script src="/js/PELocalStorage.js"></script>

<body>

<?php
  $submitBtnName = "Click to turn " .$desiredState ." Relay";
?>

<h1 style="margin-left: 5px">Test A Relay</h1>

<form id="testForm" method="post">

  @foreach ($scErrors as $error)
    <p class="alert alert-danger">{{ $error }}</p>
  @endforeach

  <!-- The next line submits the token required by Laravel -->
  <input type="hidden" name="_token" value="{!! csrf_token() !!}">
  <input type="hidden" name="noRelays" value="{!! $noRelays !!}">
  <!-- Create a hidden input to hold the action to be performed during a chnl update. -->
  <input type="hidden" id="action" name="action" value="none">
  <!-- Create a hidden input to hold the session configuration. -->
  <input type="hidden" id="peLS" name="peLS">

  <div class="form-group PELabel">
    Relay number to test:
    <select id="testRelay" name="testRelay">
      <?php
        for ($i=0; $i < $noRelays; $i++) {
          echo '<option value="' . $i . '"';
          if ($i == $testRelay - 1) {
            echo ' selected';
          }
          echo '>' . ($i + 1) . '</option>';
        }
      ?>
    </select>
  </div>
  <input class="peBtns" id="submitBtn" type="submit" name="submit" value="<?php echo $submitBtnName; ?>">
</form>

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

<script>
peLS = <?php echo json_encode($peLS); ?>      // Put the session configuration in a json string.

document.getElementById("warningStorageOKBtn").onclick = function() {
  // The user is OK with using local storage in spite of the warning.
  peLS['isUsed'] = 'true';            // This user is going to use local storage.
  loadFromLocalStorage();             // Create all the local storage space with defaults.
  document.getElementById("submitBtn").value = "none";// There is no desired state yet.
  submitForm();                       // Refresh the page with local storage.
} // End of function to allow this user to use local storage.

document.getElementById("warningStorageCancelBtn").onclick = function() {// The user doesn't want local storage.
  peLS['isUsed'] = 'false';           // Local storage is not to be used.
  document.getElementById("submitBtn").value = "none";// There is no desired state yet.
  submitForm();                       // Refresh the page without local storage.
} // End of function to decline to use of local storage.

// Add an event to respond to the user clicking the submit button.
// Start here, check to see if this works.
document.getElementById("testForm").addEventListener("submit", submitForm);

function submitForm() {               // Submit the form to the server.
  // Convert the session configuration object into a JSON string.
  document.getElementById("peLS").value = JSON.stringify(peLS);
  document.getElementById("submitBtn").click();// Submit the form to the server (PagesController).
}

function configElements() {         // The page is being drawn. Configure the pages elements.
  if(peLS['isUsed'] == 'unknown' || peLS['isUsed'] == null) {// Has the use of local storage been determined?
    checkLocalStorage();            // Unknown. Check to see if it is used. If it is, load the config.
    if(peLS['isUsed'] == 'unknown') { // Condition: Local storage been evaluated. Is local storage unknow?
      // Yes. Use the warningDlg to ask the user if they would like to use local storage.
      var warningDlg = document.getElementById("warningStorageModal");
      warningDlg.style.display = "block"; // Display the warning dialog.
    } // End of condition: Unknown usage of local storage.
  } // End of condition: The use of local storage is unknown.
}

function updateTime() {
  document.getElementById("action").value = "TurnOff";// Get the previous action.
  // Start here, finish with peLS and make sure this turns off a relay.
  document.getElementById("testForm").submit();// Submit the form to the server (PagesController).
}

window.onload = configElements;       // Configure all the configuration elements when the page loads.

window.setInterval(updateTime, 1800000);// Update the Channels page once every 1,800 seconds(30 minutes).

</script>


@endsection
