@extends('master')
@section('title', 'Home')

@section('content')

<head>

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
    /*width: 60%;*/
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
    /*padding: 12px 16px;*/
    padding: 0px 0px;
    z-index: 1;
  }

  .dropdown:hover .dropdown-content {
      display: block;
  }

  #xChnlNo {
    min-width: 70px;
  }

  </style>

</head>

<script src="/js/PELocalStorage.js"></script>

<body>

<form id="chnlForm" method="post">

  <!-- Display any errors. -->
  @foreach ($scErrors as $error)
    <p class="alert alert-danger">{{ $error }}</p>
  @endforeach

  <!-- The next line submits the token required by Laravel -->
  <input type="hidden" name="_token" value="{!! csrf_token() !!}">
  <!-- Create a hidden input to hold the action to be performed during a chnl update. -->
  <input type="hidden" id="action" name="action" value="{!! $action !!}">
  <!-- Create a hidden input to hold the chnls. -->
  <input type="hidden" id="theChnls" name="theChnls">
  <!-- Create a hidden input to hold the bed configurations. -->
  <input type="hidden" id="theBeds" name="theBeds">
  <!-- Create a hidden input to hold the stop count so the PageController can evaluate when to enable the stop button. -->
  <input type="hidden" id="stopCount" name="stopCount" value="{!! $stopCount !!}">
  <!-- Create a hidden input to hold the session configuration. -->
  <input type="hidden" id="peLS" name="peLS">

  @if (session('status'))
    {{ session('status') }}
  @endif

  <h3 class="PECenter">Channel Configuration</h3>

  <div id="chnlTable"></div>
  <div class="PECenter">
      <button class="PEBtnShow PEBtns" id="addBtn" type="button" onclick="addChnl()">Add Channel</button>
      <button class="PEBtnShow PEBtns" id="removeBtn" type="button" onclick="removeChnl()">Remove Channel</button>
      <button class="PEBtnHide PEBtns" id="saveBtn" type="button" onclick="saveClicked()">Save</button>
      <button class="PEBtnShow PEBtns" id="pmBtn" type="button" onclick="setPM()">Start the Controller</button>
      <button class="PEBtnHide PEBtns" id="cancelBtn"  type="button" onclick="location.href='{{ url('/chnls')}}'">
        Cancel Changes
      </button>
  </div>
  <p id="instruct" class="PEBtnShow PEBtns">Edit channel configurations.</p>

  <div class="PECenter">
    Cure Manager version: 1.5
    <br>
    Plant Manager version: {!! $version !!}
    <br>
    The Plant Manager last updated {!! $runTime !!} ago.
  </div>

</form>

<!-- The modal dialog to get a chnls's color. -->
<div id="chnlColorModal" class="PEmodal">
  <!-- modal content -->
  <div class="PEmodal-content">
    <h3>Set the color</h3>
    <input id="chnlColor" name="chnlColor" type="color">
    <button id="chnlColorOKBtn" class="PEBtns">OK</button>
    <button id="chnlColorCancelBtn" class="PEBtns">Cancel</button>
  </div>
</div><!-- End of the dialog to add a chnl to a bed. -->

<!-- The modal dialog to edit a chnl's name. -->
<div id="chnlNameModal" class="PEmodal">
  <!-- modal content -->
  <div class="PEmodal-content PEBorder">
    <h3>Set this channel's name</h3>
    <h4>What should this channel be called?</h4>
    <input id="chnlName" name="chnlName" type="text">
    <button id="chnlNameOKBtn" class="PEBtns">OK</button>
    <button id="chnlNameCancelBtn" class="PEBtns">Cancel</button>
  </div>
</div><!-- End of the dialog to edit a chnl name. -->

<!-- The modal dialog to edit a chnl's input. -->
<div id="chnlInputModal" class="PEmodal">
  <!-- modal content -->
  <div class="PEmodal-content PEBorder">
    <div>
      <span id="setChnlInput" class="PEDlgText">
        Set this channel's input
      </span>
      <span>
        <select class="PEDlgText" id="chnlInput" name="chnlInput"></select>
        <p></p>
      </span>
    </div>
    <div class="PECenter">
      <button id="chnlInputOKBtn" class="PEBtns">OK</button>
      <button id="chnlInputCancelBtn" class="PEBtns">Cancel</button>
    </div>
  </div>
</div><!-- End of the dialog to edit a chnl input. -->

<!-- The modal dialog to edit a chnl's relay. -->
<div id="chnlRelayModal" class="PEmodal">
  <!-- modal content -->
  <div class="PEmodal-content PEBorder">
    <div>
      <span id="setChnlRelay" class="PEDlgText">
        Set this channel's relay
      </span>
      <span>
        <select class="PEDlgText" id="chnlRelay" name="chnlRelay"></select>
        <p></p>
      </span>
    </div>
    <div class="PECenter">
      <button id="chnlRelayOKBtn" class="PEBtns">OK</button>
      <button id="chnlRelayCancelBtn" class="PEBtns">Cancel</button>
    </div>
  </div>
</div><!-- End of the dialog to edit a chnl relay. -->

<!-- The modal dialog to edit a chnl's relay. -->
<div id="chnlDeviationModal" class="PEmodal">
  <!-- modal content -->
  <div class="PEmodal-content PEBorder">
    <div class="PECenter">
      <h3>Set this channel's deviation alarm.</h3>
    </div>
    <div class="PECenter">
      <h4>What amount should this channel be allowed to deviate
      before an alarm is triggered?</h4>
    </div>
    <input type="number" id="chnlDeviation" name="chnlDeviation" min="0" max="99.9" step=".1">
    <button id="chnlDeviationRemoveBtn" class="PEBtns">Remove</button>
    <button id="chnlDeviationOKBtn" class="PEBtns">OK</button>
    <button id="chnlDeviationCancelBtn" class="PEBtns">Cancel</button>
  </div>
</div><!-- End of the dialog to edit a chnl's deviation alarm. -->

<!-- The modal dialog to edit a chnl's approach limit. -->
<div id="chnlAppLimitModal" class="PEmodal">
  <!-- modal content -->
  <div class="PEmodal-content PEBorder">
    <h3>Set this channel's approach limit.</h3>
    <h4>What should this channel's approach limit be?</h4>
    <input type="number" id="chnlAppLimit" name="chnlAppLimit" min="0" max="9.9" step=".1">
    <button id="chnlAppLimitOKBtn" class="PEBtns">OK</button>
    <button id="chnlAppLimitCancelBtn" class="PEBtns">Cancel</button>
  </div>
</div><!-- End of the dialog to edit a chnl's approach limit. -->

<!-- The modal dialog to add a chnl. -->
<div id="chnlAddModal" class="PEmodal">
  <!-- modal content -->
  <div class="PEmodal-content PEBorder">
    <h3 class="PECenter">Add a channel.</h3>
    <div class="PEDlgText PECenter">
      <label class="PEDlgText" for="chnlNo">The channel # should be:</label>
      <input type="number" id="chnlNo" name="chnlNo" min="1" step="1">
    </div>
    <div class="PECenter">
      <button id="chnlAddOKBtn" class="PEBtns">OK</button>
      <button id="chnlAddCancelBtn" class="PEBtns">Cancel</button>
    </div>
  </div>
</div><!-- End of the dialog to add a chnl. -->

<!-- The modal dialog to remove a chnl. -->
<div id="chnlRemoveModal" class="PEmodal">
  <!-- modal content -->
  <div class="PEmodal-content PEBorder">
    <h3 class="PECenter">Remove a channel.</h3>
    <div class="PEDlgText PECenter">
      <label class="PEDlgText" for="xChnlNo">The channel # to remove is:</label>
      <input type="number" id="xChnlNo" name="xChnlNo" min="1" step="1">
    </div>
    <div class="PECenter">
      <button id="chnlRemoveOKBtn" class="PEBtns">OK</button>
      <button id="chnlRemoveCancelBtn" class="PEBtns">Cancel</button>
    </div>
  </div>
</div><!-- End of the dialog to remove a chnl. -->

<!-- The modal dialog to send a warning to the user. -->
<div id="warningModal" class="PEmodal">
  <!-- modal content -->
  <div class="PEmodal-content PEWarning">
    <div class="PECenter">
      <h4 id="warningMessage">Message</h4>
    </div>
    <div class="PECenter">
      <button id="theWarningBtn" class="PEBtns">OK</button>
    </div>
  </div>
</div><!-- End of the dialog to get a yes or no response. -->

<!-- The modal dialog to send a warning to the user. -->
<div id="warningOKModal" class="PEmodal">
  <!-- modal content -->
  <div class="PEmodal-content PEWarning">
    <div class="PECenter">
      <h4 id="warningOKMessage">Message</h4>
    </div>
    <div class="PECenter">
      <button id="warningOKBtn" class="PEBtns">OK</button>
      <button id="warningCancelBtn" class="PEBtns">Cancel</button>
    </div>
  </div>
</div><!-- End of the dialog to get a yes or no response. -->

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

<!-- The modal dialog to send a warning about local storage to the user. -->
<div id="warningSaveModal" class="PEmodal">
  <!-- modal content -->
  <div class="PEmodal-content PEWarning">
    <div class="PECenter">
      <h2>WARNING!!!<h2>
    </div>
    <div class="PECenter">
      <p style="font-size:14px;">Saving the channel configuration causes the Plant Manager
      to stop and restart. It will have unpredictable effects on
      any active cure cycles.</p>
    </div>
    <div class="PECenter">
      <p style="font-size:16px;">Do you wish to continue?</p>
    </div>
    <div class="PECenter">
      <button id="warningSaveOKBtn" class="PEBtns">Save anyway</button>
      <button id="warningSaveCancelBtn" class="PEBtns">Don't save</button>
    </div>
  </div>
</div><!-- End of the dialog to get a save or don't save response. -->

<!-- Modal wait for the Plant Manager -->
<div id="waitFor" class="PEmodal">
  <!-- Modal content-->
  <div class="PEmodal-content">
    <h2 style="text-align:center;">Talking to the Plant Manager.</h2>
    <h2 style="text-align:center;">Please Wait.</h2>
  </div>
</div>

</body>

<script>
///////////////////Global/Variables//////////////////////////////////////////
  var theChnls = <?php echo json_encode($theChnls); ?>;// Put the chnl configurations into a json string.
  var theInputs = <?php echo json_encode($inputs); ?>;// Put the input configurations into a json string.
  var theRelays = <?php echo json_encode($relays); ?>;// Put the relay configurations into a json string.
  var bedConfigs = <?php echo json_encode($theBeds); ?>;// Put the bed configurations into a json string.
  var peLS = <?php echo json_encode($peLS); ?>;// Contains this session's configuration.
                                      // The default is not set until local storage is checked.
  var bColor = "White";               // Contains the backgroundColor of the dropdown behind the mouse cursor.
  var theChnlDex = -1;                // The index of the chnl being modified. (-1 indicates nothing is being modified.)
  var isConfigChange = false;         // Indicates that some chnl configuration has been chnaged.
///////////////End/Global/Variables//////////////////////////////////////////

function drawTable() {                // Displays the chnls in tabular format.
  if(peLS['isUsed'] == 'unknown' || peLS['isUsed'] == null) {// Has the use of local storage been determined?
    checkLocalStorage();              // Check to see if local storage is used.
    if(peLS['isUsed'] == 'true') {
      submitForm();
    }
    if(peLS['isUsed'] == 'unknown') { // Is the use of local storage still unknow?
      // Yes. Use the warningDlg to ask the user if they would like to use local storage.
      var warningDlg = document.getElementById("warningStorageModal");
      warningDlg.style.display = "block"; // Display the warning dialog.
    } // End of condition: The use of local storage is still unknown.
  } // End of condition: The use of local storage is unknown.
  // As of 2020-06-25 there are five items (columns). Eventually there will be a coulmn for the "Approach limit".
  // At that point the noItems needs to be changed from 5 to 6.
  var noItems = 5;
  var tableLoc = document.getElementById("chnlTable");// Get the location for the table.
  var chnlTbl = document.createElement("table");// Create the chnl table.
  chnlTbl.border = 1;                 // Give the table a visible border.
  chnlTbl.style.marginLeft = "auto";  // Center the table
  chnlTbl.style.marginRight = "auto"; // in the horizontal direction.
  chnlTbl.setAttribute("id", "theChnlTable");// Allow access to the table through its id.
  var hRow = chnlTbl.insertRow(-1);   // Create a header row in the table.
  var hTxt = ["#", "Name", "Input #", "Relay #", "Deviation", "Approach Limit"];// Labels for the header row.
  for(var c = 0; c < noItems; c++) {  // Create a cell for each of the six columns.
    var hCell = document.createElement("th");// Create the header cell.
    hCell.style.padding = "6px";      // Create space around the label for readability.
    if(c == 0 ) {                     // Is this the chnl number column?
      hCell.style.paddingLeft = "20px";// Yes. Add extra space to the left
      hCell.style.paddingRight = "20px";// and to the right to allow for larger chnl names.
    } else if(c == 1) {               // Is this the chnl name column?
      hCell.style.paddingLeft = "35px";// Yes. Add extra space to the left
      hCell.style.paddingRight = "35px";// and to the right to allow for larger bed chnl names.
    } // End of condition chnl name column.
    hCell.innerHTML = hTxt[c];        // Set this column's header.
    hRow.appendChild(hCell);          // Add this column's header cell to the header row.
  } // End of looping through each column to set its label.
  for(var chnlDex = 0; chnlDex < theChnls.length; chnlDex++) {// Go through each chnl.
    var row = document.createElement("tr");// Create a row in the chnl table to show this chnl's configuration.
    var scChnl = theChnls[chnlDex];   // Get this chnls scChnl configuration.
    var colorTxt = peLS['PE_Chnl_Color'][chnlDex];// Get this chnl's color.
    for(var colDex = 0; colDex < noItems; colDex++) {// Loop through each column.
      var cell = document.createElement("td");// Create a cell to hold part of this chnl's configuration.
      if (chnlDex % 2 == 0) {         // Is this a chnl with an even index?
        cell.style.backgroundColor = "gainsboro";// Yes. Change the backgroundColor so the table is easier to read.
      } else {                    // This is a chnl with an odd index.
        cell.style.backgroundColor = "white";// Change the backgroundColor to be different than the beds with an even index.
      } // End of condition to set every other row a different backgroundColor.

      if(colDex == 0) {               // Is this the column for the chnl's id?
        var cellText = document.createTextNode(scChnl.id);// Create a text node to hold this chnl's id.
        cell.style.textAlign = "center";// Center the id on the table's cell.
        cell.appendChild(cellText);   // Add the chnl's id to the table's cell.
        cell.onmouseup = function(){editColor(event)};// Allow the user to modify this chnl's color.
      } else if (colDex == 1) {       // Is this the column for the chnl's name?
        var cellText = document.createTextNode(scChnl.name);// Put its name in a text node.
        cell.style.textAlign = "center";// Center the chnl's name in the cell.
        cell.appendChild(cellText);   // Make the chnl's name a part of this table cell.
        cell.onmouseup = function(){editChnlName(event)};// Allow the user to modify this chnl's name.
      } else if (colDex == 2) {        // Is this the column for the chnl's input number?
        var text = document.createTextNode(scChnl.inputNo);// Create its text.
        cell.style.textAlign = "center";// Center the input number in the table cell.
        cell.appendChild(text);       // Add the input number to the table's cell.
        cell.onmouseup = function(){editChnlInput(event)};// Allow the user to modify this chnl's input.
      } else if (colDex == 3) {       // Is this the column for the chnl's relay number?
        var rText = "none";           // Yes. By default there is no relay assigned to this chnl.
        if(scChnl.relayNo > 0) {      // Has relay been assigned to this chnl?
          rText = scChnl.relayNo.toString();// Yes. Change the cell to show the relay number.
        } // End of condition looking for an assigned relay.
        var text = document.createTextNode(rText);// Create this cell's text.
        cell.style.textAlign = "center";// Center the relay number in the table cell.
        cell.appendChild(text);       // Add the relay number to the table's cell.
        cell.onmouseup = function(){editChnlRelay(event)};// Allow the user to modify this chnl's relay.
      } else if (colDex == 4) {       // Is this the column for the chnl's deviation alarm?
        var dev = scChnl.deviation;   // Yes. Get the allowable deviation for this chnl.
        var devTxt = "none";          // The default is that a deviation limit is not set.
        if(dev > 0) {                 // Is the allowable deviation positive?
          devTxt = dev.toString();    // There is a deviation limit.
        } // End of conditional check for a deviation limit.
        var text = document.createTextNode(devTxt);// Create its text.
        cell.style.textAlign = "center";// Center the deviation limit in the table cell.
        cell.appendChild(text);       // Add the deviation limit to the table's cell.
        cell.onmouseup = function(){editChnlDeviation(event)};// Allow the user to modify this deviation alarm.
      } else {                        // Create a cell to contain the chnl's approach limit.
        var text = document.createTextNode(scChnl.approachLimit.toString());// Create its text.
        cell.style.textAlign = "center";// Center the deviation limit in the table cell.
        cell.appendChild(text);       // Add the deviation limit to the table's cell.
        cell.onmouseup = function(){editChnlAppLimit(event)};// Allow the user to modify this approach limit.
      }                           // Each chnl's four configuration items is in a cell.
      cell.style.color = colorTxt;   // Set the color for the chnl.
      cell.setAttribute("id", chnlDex + "-" + colDex);// Set the id attribute so this cell can be accessed.
      cell.onmouseover = function(){enterCell(event)};// Set the cell's highlighting when the mouse is over this cell.
      cell.onmouseleave = function(){cellLeave(event)};// Remove the cell's highlighting when the mouse is not over it.
      row.appendChild(cell);          // Add this cell to this table row.
    } // End of looping through each column.
    chnlTbl.appendChild(row);         // Add this chnl's row to the table.
  } // End of looping through each chnl.
  chnlTable.appendChild(chnlTbl);     // Put the table on the page.
  configPMBtn();                      // Configure the buttons for starting & stopping the Plant Manager.
  setSaveBtn();                       // Display the save & cancel btns if necessary.
} // End of function drawTable().

function redrawTable() {
  var chnlDiv = document.getElementById("chnlTable");// Get the parent for the chnl table.
  var cT = document.getElementById("theChnlTable");// Get the bed table.
  chnlDiv.removeChild(cT);           // Remove the bed table from its parent.
  drawTable();                      // Recreate the bed table which now includes the new bed.
}

function enterCell(event) {         // The cursor is entering a cell. Highlight it.
  var bgc = event.target.style.backgroundColor;// Get the cell's current background color.
  if (bgc != "lightgreen" && bgc != "") {// Is the backgound color already a highlight or undefined?
    bColor = event.target.style.backgroundColor;// Yes. Save the cell's current background color.
  } // End of condition checking if the cell is already highlighted.
  event.target.style.backgroundColor = "lightgreen";// Set the background color to lightgreen to highlight it.
  event.currentTarget.style.backgroundColor = "lightgreen";// Set the background color to lightgreen to highlight it.
}

function cellLeave(event) {         // Remove the cell's highlighting.
  event.currentTarget.style.backgroundColor = bColor;// Retrun the background color to its original value.
}

function addChnl() {                  // The user wants to add a chnl.
  var theChnlNo = document.getElementById("chnlNo");// Access the input which contains the chnl's number.
  theChnlNo.value = theChnls.length + 1;// The default chnl number to add is one greater than the last chnl.
  theChnlNo.max = theChnls.length + 1;// The chnlNo control must have its maximum increased.
  document.getElementById("chnlAddModal").style.display = "block";// Display the modal dialog to add a chnl.
  theChnlNo.focus();               // Set the focus to the input used to edit the chnl's approach limit.
} // End of function to start adding a channel.

document.getElementById("chnlAddCancelBtn").onclick = function() {// The user decided not to add a chnl.
  document.getElementById("chnlAddModal").style.display = "none";// Hide the add chnl dialog.
}

document.getElementById("chnlAddOKBtn").onclick = function() {// The user is OK with the chnl number to add.
  var cNo = Number(document.getElementById("chnlNo").value);// Get the user's desired chnl number.
  if(cNo > theChnls.length + 1 || cNo < 1) {// Is this chnl number valid?
    cNo = theChnls.length + 1;        // No. Set the chnl number to one greater than the last chnl number.
  } // End of condition to adjust an invalid chnl number.
  var cNoForName = cNo;               // This variable will be used to create the name for the chnl being added.
  var name = cNoForName.toString();   // Create a chnl name based on the chnl number.
  var isAMatch = true;                // Start out assuming there is a matching name until all names have been checked.
  while(isAMatch == true) {           // While there is a match go through chnls chnging the name until there isn't a match.
    for(cDex = 0; cDex < theChnls.length; cDex++) {// Go through each chnl.
      var aChnl = theChnls[cDex];     // Access one of the chnls.
      if(aChnl.name == name) {        // is its name the same as the name created for the chnl being added?
        cNoForName++;                 // Yes. Increment the number being used to create the name.
        name = cNoForName.toString(); // Create the new name.
        break;                        // There is a name match. A new name has been created, go back and check for matches.
      } // End of condition of a name match.
    } // End of looping to find a matching name.
    if(cDex >= theChnls.length) {     // Have all the existing chnl names been compared to the new name?
      isAMatch = false;               // Yes. And there wasn't a name match. The created name is unique.
    } // End of condition of no name match.
  } // End of loop to go through all the chnls checking for a name match with the created name.
  var scChnl = {id: cNo};             // Start creating an scChnl object.
  scChnl.name = name;                 // Set the scChnl's name.
  scChnl.deviation = "0.0";           // Set the scChnl's deviation.
  scChnl.approachLimit = "0";         // Set the scChnl's approach limit.
  scChnl.inputNo = "1";               // Set the scChnl's input number.
  scChnl.relayNo = "0";               // Set the scChnl's relay number.
  console.log(scChnl);
  theChnls.splice(cNo - 1, 0, scChnl);//Insert this new chnl in the correct position.
  for(var cDex = cNo; cDex < theChnls.length; cDex++) {// Loop through the chnls after the added chnl to adjust their ids.
    console.log("cDex = " + cDex);
    theChnls[cDex].id = cDex + 1;     // Add one to the id to accommodate the chnl insert in front of this one.
  } // End of looping to adjust chnl ids.
  // Go through the beds and adjust the scChnl indexes after the one added.
  for(var bDex = 0; bDex < bedConfigs.noBeds; bDex++) {// Go through each bed.
    var bTxt = "bed" + bDex;          // Used to access this bed from the bed configurations passed by the controller.
    var noChnls = bedConfigs[bTxt].noChnls;// Get this bed's number of chnls.
    for(var cDex = 0; cDex < noChnls; cDex++){// Go through this bed's chnls.
      var bcTxt = "bedChnl" + cDex;   // Used to access this bed chnl's configuration passed by the controller.
      var bedChnl = bedConfigs[bTxt][bcTxt];// Used to access this bed chnl's configuration.
      var scChnlDex = Number(bedChnl.scChnlDex);// Contains this bed chnl's scChnl index.
      var scChnlNo = scChnlDex + 1;   // The chnl number is ones based.
      if(scChnlNo >= cNo) {           // Is this bed chnl greater than the removed chnl?
        // Yes. Decrement the scChnlDex for this bed chnl.
        bedConfigs[bTxt][bcTxt].scChnlDex++;// This increments the chnl's index.
      } // End of condition checking if this bed chnl is greater than the one the user wants to remove.
    } // End of looping through each bed chnl.
  } // End of looping through each bed.
  document.getElementById("chnlAddModal").style.display = "none";// Close the add chnl dialog box.
  isConfigChange = true;              // There has been a configuration change.
  pageReset();                        // Regenerate the chnl page.
} // End of function to finish adding a chnl.

function removeChnl() {               // The user wants to remove a chnl.
  var theChnlNo = document.getElementById("xChnlNo");// Access the text input which contains the chnl's number.
  theChnlNo.value = theChnls.length;  // By default suggest removing the last channel.
  theChnlNo.max = theChnls.length;    // There can be no more than the number of channels to remove.
  document.getElementById("chnlRemoveModal").style.display = "block";// Display the modal dialog to set a chnl's approach limit.
  theChnlNo.focus();               // Set the focus to the input used to edit the chnl's approach limit.
} // End of function to start removing a chnl.

function saveClicked() {              // The user wants to save the changes.
  // Saving the changes requires stopping and then restarting the PM. The chnl config effects many items in the PM.
  var saveDlg = document.getElementById("warningSaveModal");// Use the warningDlg to alert the user about restarting the PM.
  saveDlg.style.display = "block";    // Display the warning dialog.
}

function configPMBtn() {
  // Get the string which starts with the number of minutes since the last PM update.
  var minutesSincePMUpdate = parseInt("{!! $runTime !!}");
  console.log("minutesSincePMUpdate: " + minutesSincePMUpdate);
  var pmBtn = document.getElementById("pmBtn");// Access the button to start the PM.
  var theAction = document.getElementById("action").value;
  console.log("theAction: " + theAction);
  if(minutesSincePMUpdate >= 2) {     // Has it been at least 2 minutes since the PM updated its runTime?
    pmBtn.innerHTML = "Start the Controller";// Change the button label to allow a start.
    pmBtn.disabled = false;           // Yes. Allow the user to start the PM.
    pmBtn.style.color = "black";      // Black makes the button look enabled.
  } else {                            // It has been less than 2 minutes since the PM updated its runTime.
    pmBtn.disabled = true;            // Don't allow the user to start the PM. It might be OK to stop the PM.
    pmBtn.style.color = "lightgray";  // Lightgray makes the button look disabled.
    console.log("Stop Count: " + document.getElementById("stopCount").value);
    if("{!! $stopCount !!}" > 3) {    // Have there been at least three updates since the PM started?
      pmBtn.innerHTML = "Stop the Controller";// Change the button label to allow a start.
      pmBtn.disabled = false;         // Allow the user to shut down the PM.
      pmBtn.style.color = "black";    // Black makes the button look enabled.
    } // End of condition: Evaulate the number of updates and then set the state of the Plant Manager button.
  } // End of condition to set the state of the plant manager button.
} // End of function configPMBtn.

function setPM() {                    // The user wants to start the PM.
  var pmBtn = document.getElementById("pmBtn");// Access the button to start the PM.
  if(pmBtn.innerHTML == "Start the Controller") {// Does the PM need to be started?
    document.getElementById("action").value = "Start";// Yes. Pass the 'start PM' command to the PagesController.
    pmBtn.disabled = true;            // Don't let the user click the start button twice.
    pmBtn.style.color = "lightgray";  // Lightgray makes the button look disabled.
    submitForm();                     // Submit the "Start" action to the page controller to start the PM.
  } else {                            // The PM needs to be stopped.
    var warningDlg = document.getElementById("warningOKModal");// Yes. Use the warningDlg to alert the user.
    var message = "<p>This will stop the Plant Manger! Do you wish to continue?</p>";// Create
    document.getElementById("warningOKMessage").innerHTML = message;// Set the reason.
    warningDlg.style.display = "block"; // Display the warning dialog.
  } // End of condition: The PM needs to be stopped.
}

document.getElementById("warningOKBtn").onclick = function() {// The user is OK with the action in spite of the warning.
  var warningDlg = document.getElementById("warningOKModal");// Get acces to the warning dialog.
  document.getElementById("action").value = "Stop";// The PM should be stopped?
  warningDlg.style.display = "none";    // Yes. Remove the warning dialog.
  var pmBtn = document.getElementById("pmBtn");// Access the stop PM button.
  pmBtn.disabled = true;              // Don't let the user click the stop button twice.
  pmBtn.style.color = "lightgray";    // Lightgray makes the button look disabled.
  submitForm();                       // Submit the "Stop" action to the PagesController to stop the PM.
  document.getElementById("waitFor").style.display = "block";// Stopping takes a while. Alert the user.
} // End of function to accept the action which caused the warning.

document.getElementById("warningSaveOKBtn").onclick = function() {// The user is OK with saving in spite of the warning.
  var warningDlg = document.getElementById("warningSaveModal");// Get acces to the warning dialog.
  document.getElementById("action").value = "save";// The chnl config should be saved and thus the PM stopped & started.
  warningDlg.style.display = "none";  // Remove the warning dialog.
  var pmBtn = document.getElementById("pmBtn");// Access the stop PM button.
  pmBtn.disabled = true;              // Don't let the user click the stop button twice.
  pmBtn.style.color = "lightgray";    // Lightgray makes the button look disabled.
  submitForm();                       // Submit the save action to the PagesController.
  document.getElementById("waitFor").style.display = "block";// Saving takes a while. Alert the user.
} // End of function, the user wants to save chnl config changes even though the PM will be stopped & started.

document.getElementById("warningSaveCancelBtn").onclick = function() {// Cancel the action associated with the warning.
  // Pass the 'UpdateRevert' command to the PagesController to load the existing chnl config (action cancelled).
  document.getElementById("action").value = "UpdateRevert";
  setSaveBtn();                       // Remove the Save and Cancel buttons.
  submitForm();                       // Submit the "UpdateRevert" action to the PagesController.
} // End of function, the user doesn't want to save chnl config changes.

document.getElementById("warningStorageOKBtn").onclick = function() {// The user is OK with the action in spite of the warning.
  peLS['isUsed'] = 'true';            // This user is going to use local storage.
  loadFromLocalStorage();             // Create all the local storage space with defaults.
  submitForm();                       // Submit the changes to the server.
} // End of function to allow this user to use local storage.

document.getElementById("warningStorageCancelBtn").onclick = function() {// The user doesn't want local storage.
  peLS['isUsed'] = 'false';           // Local storage is not to be used.
  submitForm();                       // Submit the desired storage.
} // End of function to decline to use of local storage.

document.getElementById("warningCancelBtn").onclick = function() {// Cancel the action associated with the warning.
  document.getElementById("action").value = "Update0";// Pass the 'Update' command to the controller (action cancelled).
  submitForm();
} // End of function to accept the action which caused the warning.

document.getElementById("chnlRemoveCancelBtn").onclick = function() {// The user decided not to remove a chnl.
  document.getElementById("chnlRemoveModal").style.display = "none";
}

document.getElementById("chnlRemoveOKBtn").onclick = function() {// The user is OK with the chnl number to remove.
  var cNo = Number(document.getElementById("xChnlNo").value);// Get the chnl number the user wants to remove.
  var cName = theChnls[cNo - 1].name;
  console.log("The chnl name is " + cName);
  if(cNo > theChnls.length + 1 || cNo < 1) {// Is this chnl number valid?
    // Display the warning dialog.
    var warningDlg = document.getElementById("warningModal");// Yes. Use the warningDlg to alert the user.
    var message = "<p>" + cNo.toString() + " is not a valid channel number.</p>";// Create
    document.getElementById("warningMessage").innerHTML = message;// Set the reason.
    warningDlg.style.display = "block";// Display the warning dialog.
    return;                           // There is nothing else to do here.
  } // End of condition to warn about an invalid chnl number.
  for(var bDex = 0; bDex < bedConfigs.noBeds; bDex++) {// Go through each bed.
    var bTxt = "bed" + bDex;          // Used to access this bed from the bed configurations passed by the controller.
    var noChnls = bedConfigs[bTxt].noChnls;// Get this bed's number of chnls.
    for(var cDex = 0; cDex < noChnls; cDex++){// Go through this bed's chnls.
      var bcTxt = "bedChnl" + cDex;   // Used to access this bed chnl's configuration passed by the controller.
      var bedChnl = bedConfigs[bTxt][bcTxt];// Used to access this bed chnl's configuration.
      var scChnlDex = Number(bedChnl.scChnlDex);// Contains this bed chnl's scChnnl index.
      var scChnlNo = scChnlDex + 1;   // The chnl number is ones based.
      if(cNo == scChnlNo) {           // Is this bed chnl the same as the chnl to be removed.
        // Yes. Display the warning dialog.
        var warningDlg = document.getElementById("warningModal");// Use the warningDlg to alert the user.
        var message = "<p>Channel " + cName;// Create a message stating why this channel cannot be removed.
        message += " cannot be removed until it is removed from bed " + bedConfigs[bTxt].name + ".</p>";
        document.getElementById("warningMessage").innerHTML = message;// Set the reason.
        warningDlg.style.display = "block";// Display the warning dialog.
        document.getElementById("chnlRemoveModal").style.display = "none";// Close the remove chnl dialog box.
        pageReset();                        // Regenerate the chnl page.
        return;                           // There is nothing else to do here.
      } // End of condition checking if this bed chnl is the same as the one the user wants to remove.
    } // End of looping through each bed chnl.
  } // End of looping through each bed.
  theChnls.splice(cNo - 1, 1);        // Remove this chnl from the chnl array.
  for(var cDex = cNo - 1; cDex < theChnls.length; cDex++) {// Loop through the chnls after the removed chnl to adjust their ids.
    theChnls[cDex].id = cDex + 1;     // Subtract one from the id to accommodate the removed chnl in front of this one.
  } // End of looping to adjust chnl ids.
  // Go through the beds and adjust the scChnl indexes after the one removed.
  for(var bDex = 0; bDex < bedConfigs.noBeds; bDex++) {// Go through each bed.
    var bTxt = "bed" + bDex;          // Used to access this bed from the bed configurations passed by the controller.
    var noChnls = bedConfigs[bTxt].noChnls;// Get this bed's number of chnls.
    for(var cDex = 0; cDex < noChnls; cDex++){// Go through this bed's chnls.
      var bcTxt = "bedChnl" + cDex;   // Used to access this bed chnl's configuration passed by the controller.
      var bedChnl = bedConfigs[bTxt][bcTxt];// Used to access this bed chnl's configuration.
      var scChnlDex = Number(bedChnl.scChnlDex);// Contains this bed chnl's scChnnl index.
      var scChnlNo = scChnlDex + 1;   // The chnl number is ones based.
      if(scChnlNo >= cNo) {           // Is this bed chnl greater than the added chnl?
        // Yes. Increment the scChnlDex for this bed chnl
        bedConfigs[bTxt][bcTxt].scChnlDex--;// This decrements the chnl's index.
      } // End of condition checking if this bed chnl is greater than the one the user wants to remove.
    } // End of looping through each bed chnl.
  } // End of looping through each bed.
  document.getElementById("chnlRemoveModal").style.display = "none";// Close the remove chnl dialog box.
  isConfigChange = true;              // There has been a configuration change.
  pageReset();                        // Regenerate the chnl page.
} // End of function to finish removing a chnl.

function editColor(event) {            // The user wants to change a chnl's color.
  var dash = event.target.id.indexOf("-");//The chnlDex is the number before the "-".
  theChnlDex = Number(event.target.id.substr(0, dash));// Extract the chnlDex.
  var scChnl = theChnls[theChnlDex];  // Access the chnl by using its index.
  var colorTxt = peLS['PE_Chnl_Color'][theChnlDex];// Get this chnl's color.
  document.getElementById("chnlColor").value = colorTxt;// Set the color control to this chnl's color.
  document.getElementById("chnlColorModal").style.display = "block";// Display the dialog to edit a chnl's color.
}

document.getElementById("chnlColorCancelBtn").onclick = function() {// The user doesn't want to change the color.
  document.getElementById("chnlColorModal").style.display = "none";// Remove the color dialog box.
}

document.getElementById("chnlColorOKBtn").onclick = function() {
  // The HTML color control stores the color as a hex number. Get that hex number.
  var theChnlColor = document.getElementById("chnlColor").value;
  peLS['PE_Chnl_Color'][theChnlDex] = theChnlColor;// Set this chnl's color for this session.
  if(peLS['isUsed'] == 'true') {      // Does this user allow local storage?
    // Yes. Set this chnl's color in local storage.
    localStorage.setItem('PE_Chnl' + theChnlDex.toString() + '_Color', theChnlColor);
  } // End of condition: This user allow local storage.
  // Tell the server to update the session configuration.
  document.getElementById("action").value = "Update0";
  submitForm();                       // Submit the form to the server.
}

function editChnlName(event) {        // The user wants to change a chnl's name.
  var dash = event.target.id.indexOf("-");//The chnlDex is the number before the "-".
  theChnlDex = Number(event.target.id.substr(0, dash));// Extract the chnlDex.
  var scChnl = theChnls[theChnlDex];  // Access the chnl by using its index.
  var tBoxChnlName = document.getElementById("chnlName");// Access the text box which will contain the chnl's name.
  tBoxChnlName.value = scChnl.name;   // Set the current chnl name in the text box.
  document.getElementById("chnlNameModal").style.display = "block";// Display the modal dialog to set a chnl's name.
  tBoxChnlName.focus();               // Set the focus to the text box used to edit the chnl's name.
}

document.getElementById("chnlNameCancelBtn").onclick = function() {// The user doesn't want to change the name.
  document.getElementById("chnlNameModal").style.display = "none";// Remove the name dialog box.
}

document.getElementById("chnlNameOKBtn").onclick = function() {// The user is OK with the chnl name.
  // Check to be sure the user's chnl name hasn't already been used.
  var desiredName = document.getElementById("chnlName").value;// Get the user's desired chnl name.
  for(var aChnlDex = 0; aChnlDex < theChnls.length; aChnlDex++) {// Loop through each chnl looking for a name match.
    if(aChnlDex == theChnlDex) {      // Is this the chnl whose name is being changed?
      continue;                       // Yes. Don't compare a chnl against itself.
    } // End of condition checking if the chnl is being compared against itself.
    if( desiredName == theChnls[aChnlDex].name) { // Does this chnl's name match the desired name?
      var warningDlg = document.getElementById("warningModal");// Yes. Use the warningDlg to alert the user.
      var message = "<p>" + desiredName + " matches the name of channel number " + (aChnlDex + 1).toString() + ".</p>";// Create
      message += "<p>Duplicate names are not allowed!</p>";// the reason the name can't be used.
      document.getElementById("warningMessage").innerHTML = message;// Set the reason.
      warningDlg.style.display = "block";// Display the warning dialog.
      return;                         // There is nothing else to do here.
    } // End of condition checking if this chnl's name matches the desired name.
  } // End of loop through each chnl looking for a name match.
  theChnls[theChnlDex].name = document.getElementById("chnlName").value;// Set this chnl's name.
  document.getElementById("chnlNameModal").style.display = "none";// Close the chnl name dialog box.
  isConfigChange = true;              // There has been a configuration change.
  pageReset();                        // Redraw the chnl table with the new chnl name.
} // End of function to process a chnl name change.

function editChnlInput(event) {       // The user wants to change a chnl's input.
  var dash = event.target.id.indexOf("-");//The chnlDex is the number before the "-".
  theChnlDex = Number(event.target.id.substr(0, dash));// Extract the chnlDex.
  // Set the label to include the chnl number so the user can more easily select the input.
  document.getElementById("setChnlInput").innerHTML = "The channel " + (theChnlDex + 1).toString() + " input should be: "
  var scChnl = theChnls[theChnlDex];  // Access the chnl by using its index.
  for(var inDex  = 0; inDex < theInputs.length; inDex++) {// Go through each input.
    if(theInputs[inDex].units != 1 && theInputs[inDex].units != 2) {// Does thisinput measure a tmeprature?
      continue;                       // No. This input does not measure temperatures. Don't add it to the list.
    } // End of condition for adding inputs used to measure temperatures to the list.
    var opt = document.createElement("option");// Create the choice.
    var text = document.createTextNode((inDex + 1).toString());// Create its text.
    if(inDex + 1 == scChnl.inputNo) { // Is this the chnl's input?
      opt.selected = true;            // Yes. Set its "selected" property.
    } // End of condition checking if this is this chnl's input.
    opt.appendChild(text);            // Add the text to the choice.
    document.getElementById("chnlInput").appendChild(opt);// Add the choice to the selector.
  } // End of loop through each input.
  document.getElementById("chnlInputModal").style.display = "block";// Display the modal dialog to set a chnl's name.
}

document.getElementById("chnlInputCancelBtn").onclick = function() {// The user decided not to chnge the chnl's input.
  document.getElementById("chnlInputModal").style.display = "none";
}

document.getElementById("chnlInputOKBtn").onclick = function() {// The user is OK with the new chnl input.
  var desiredInput = Number(document.getElementById("chnlInput").value);// Get the user's desired input.
  theChnls[theChnlDex].inputNo = desiredInput;// Set this chnl's input.
  document.getElementById("chnlInputModal").style.display = "none";// Close the chnl name dialog box.
  isConfigChange = true;             // There has been a configuration change.
  pageReset();                      // Redraw the chnl table with the new chnl input.
} // End of function to process a chnl input change.

function editChnlRelay(event) {       // The user wants to change a chnl's relay.
  var dash = event.target.id.indexOf("-");// The chnlDex is the number before the "-".
  theChnlDex = Number(event.target.id.substr(0, dash));// Extract the chnlDex.
  var scChnl = theChnls[theChnlDex];  // Access the chnl by using its index.
  var noAvailableRelays = 0;          // Contains the number of relays which can be selected.
  // Go through each relay and determine if it is already assigned to a chnl.
  // If a relay is assigned to a chnl, it cannot be part of the list.
  // If this chnl has a relay, the first option is to remove the relay.
  if(scChnl.relayNo > 0 && scChnl.relayNo <= theRelays.length) {// Does this chnl have a relay?
    var opt = document.createElement("option");// Yes. Create the choice.
    var text = document.createTextNode("Remove");// Create its text.
    opt.appendChild(text);            // Add the text to the choice.
    document.getElementById("chnlRelay").appendChild(opt);// Add the choice to the selector.
  } // End of condition to set the remove relay option.
  for(var rNo  = 1; rNo <= theRelays.length; rNo++) {// Go through each relay.
    var chnlDex = 0;                  // Begining with the first chnl
    for(; chnlDex < theChnls.length; chnlDex++) {// loop through all the chnls.
      if(theChnls[chnlDex].relayNo == rNo) {// Is this relay assigned to a chnl?
        if(theChnlDex == chnlDex) {   // Yes. Is the chnl it is assigned to the chnl being modified?
          continue;                   // Yes. This relay is allowed to be on the list.
        } else {// The relay is assigned to another chnl.
          break;                      // This relay cannot be on the selection list.
        } // End of condition for a relay which is assigned.
      } // End of condition of a relay already assigned.
    } // End of looping through all the chnls.
    if(chnlDex >= theChnls.length) {  // Were all the chnls examined for a match to this relay?
      var opt = document.createElement("option");// Yes. This relay hasn't been assigned. Thus, this relay can be selected.
      var text = document.createTextNode(rNo.toString());// Create its text.
      if(rNo == scChnl.relayNo) {     // Is this the chnl's relay?
        opt.selected = true;          // Yes. Set its "selected" property.
      } // End of condition checking if this relay is assigned to this chnl.
      noAvailableRelays++;            // Increment the count of the number of relays on the selection list.
      opt.appendChild(text);          // Add the text to the choice.
      document.getElementById("chnlRelay").appendChild(opt);// Add the choice to the selector.
    } // End of condition for adding a relay number to the list.
  } // End of loop through each relay.
  // Set the label as if there are no relays to choose from.
  var dlgText = "All relays have been assigned to channels.<br>";
  dlgText += "A relay must be removed from a channel before one can be assigned to this channel."
  if(noAvailableRelays > 0) {         // Is there at least one relay to select?
    dlgText = "The channel " + (theChnlDex + 1).toString() + " relay should be: ";// Yes. Set the label to ask for the relay.
  } // End of the condition that there is at least one relay to select.
  document.getElementById("setChnlRelay").innerHTML = dlgText;// Set the label.
  document.getElementById("chnlRelayModal").style.display = "block";// Display the modal dialog to set a chnl's relay.
} // End of function editChnlRelay.

document.getElementById("chnlRelayCancelBtn").onclick = function() {// The user decided not to chnge the chnl's relay.
  document.getElementById("chnlRelayModal").style.display = "none";// Hide the relay change dialog.
}

document.getElementById("chnlRelayOKBtn").onclick = function() {// The user is OK with the new chnl relay.
  var desiredRelay = 0;               // The default answer is to remove the relay.
  var selection = document.getElementById("chnlRelay").value;// Get the user's desired relay.
  if(selection != "Remove") {         // Does the user want to remove this relay from this chnl?
    desiredRelay = Number(selection); // No. Get the desired relay to associate with this chnl.
  } // End of condition: User doesn't want to remove this chnl's relay.
  theChnls[theChnlDex].relayNo = desiredRelay;// Set this chnl's relay.
  document.getElementById("chnlRelayModal").style.display = "none";// Close the chnl relay dialog box.
  document.getElementById("action").value = "Update0";// Set the action to 'Update' to display save & cancel btns.
  isConfigChange = true;              // There has been a configuration change.
  setSaveBtn();                       // Display the Save and Cancel Changes buttons.
  pageReset();                        // Display the page with the new changes.
} // End of function to process a chnl relay change.

function editChnlDeviation(event) {   // The user wants to change a chnl's deviation alarm.
  var dash = event.target.id.indexOf("-");//The chnlDex is the number before the "-".
  theChnlDex = Number(event.target.id.substr(0, dash));// Extract the chnlDex.
  var scChnl = theChnls[theChnlDex];  // Access the chnl by using its index.
  var inputChnlDev = document.getElementById("chnlDeviation");// Access the input which contains the chnl's deviation.
  var usedDev = scChnl.deviation;     // Get this chnl's current deviation alarm.
  if(usedDev == 0) {                  // Is the deviation alarm off?
    usedDev = 5;                      // Yes. Suggest 5 for a deviation alarm.
  } // End of condition: This chnl's deviation alarm is off.
  inputChnlDev.value = usedDev.toString();// Set the current chnl deviation in the text box.
  document.getElementById("chnlDeviationModal").style.display = "block";// Display the modal dialog to set a chnl's deviation.
  inputChnlDev.focus();               // Set the focus to the input used to edit the chnl's deviation.
} // End of function editChnlDeviation.

document.getElementById("chnlDeviationCancelBtn").onclick = function() {// The user decided not to chnge the chnl's deviation.
  document.getElementById("chnlDeviationModal").style.display = "none";// Hide the deviation dialog box.
}

document.getElementById("chnlDeviationOKBtn").onclick = function() {// The user is OK with the chnl's new deviation.
  var dev = document.getElementById("chnlDeviation").value;// Get the user's desired deviation.
  theChnls[theChnlDex].deviation = dev;// Set this chnl's deviation. theChnlDex is global and set in editChnlDeviation.
  document.getElementById("chnlDeviationModal").style.display = "none";// Close the chnl deviation dialog box.
  isConfigChange = true;              // There has been a configuration change.
  pageReset();                        // Regenerate the chnl page.
} // End of function to process a chnl deviation change.

document.getElementById("chnlDeviationRemoveBtn").onclick = function() {// The user wants to remove the chnl's deviation alarm.
  theChnls[theChnlDex].deviation = 0; // Set this chnl's deviation to off (0).
                                      // theChnlDex is global and set in editChnlDeviation.
  document.getElementById("chnlDeviationModal").style.display = "none";// Close the chnl deviation dialog box.
  isConfigChange = true;              // There has been a configuration change.
  pageReset();                        // Regenerate the chnl page.
} // End of function to remove a chnl deviation alarm.

function editChnlAppLimit(event) {   // The user wants to change a chnl's approach limit.
  var dash = event.target.id.indexOf("-");//The chnlDex is the number before the "-".
  theChnlDex = Number(event.target.id.substr(0, dash));// Extract the chnlDex.
  var scChnl = theChnls[theChnlDex];  // Access the chnl by using its index.
  var inputChnlAL = document.getElementById("chnlAppLimit");// Access the input which contains the chnl's approach limit.
  inputChnlAL.value = scChnl.approachLimit.toString();// Set the current chnl approach limit in the input.
  document.getElementById("chnlAppLimitModal").style.display = "block";// Display the modal dialog to set a chnl's approach limit.
  inputChnlAL.focus();                // Set the focus to the input used to edit the chnl's approach limit.
} // End of function editChnlAppLimit.

document.getElementById("chnlAppLimitCancelBtn").onclick = function() {// The user decided not to chnge the chnl's approach limit.
  document.getElementById("chnlAppLimitModal").style.display = "none";
}

document.getElementById("chnlAppLimitOKBtn").onclick = function() {// The user is OK with the chnl's new approach limit.
  var aL = document.getElementById("chnlAppLimit").value;// Get the user's desired approach limit.
  theChnls[theChnlDex].approachLimit = aL;// Set this chnl's approach limit.
  document.getElementById("chnlAppLimitModal").style.display = "none";// Close the chnl approach limit dialog box.
  isConfigChange = true;              // There has been a configuration change.
  pageReset();                        // Regenerate the chnl page.
} // End of function to process a chnl approach limit change.

// Add an event to respond to the user clicking the submit button.
document.getElementById("chnlForm").addEventListener("submit", submitFunction);

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

function hexToHsl(hex) {              // Convert the hex color number into hsl color stored as a JSON string.
  if(hex.charAt(0) == "#") {          // Is the first chartacter #? # just indicates hex, it is not a part of the hex number.
      hex = hex.substr(1, 6);         // Yes. Remove the # from the hex string.
  } // End of the condition checking to remove a # from the color's hex representation.
  var r = "0x" + hex.substr(0, 2);    //Convert the first two hex characters to a hex number for red.
  var g = "0x" + hex.substr(2, 2);    //Convert the next two hex characters to a hex number for green.
  var b = "0x" + hex.substr(4, 2);    //Convert the next two hex characters to a hex number for blue.

  var r2 = parseInt(r, 16);           // Convert the red hex number into a red integer.
  var g2 = parseInt(g, 16);           // Convert the green hex number into a red integer.
  var b2 = parseInt(b, 16);           // Convert the blue hex number into a red integer.

  r2 /= 255;   //Convert the red value to anumber between 0 and 1 (currently is 0 and 255).
  g2 /= 255;   //Convert the green value to anumber between 0 and 1 (currently is 0 and 255).
  b2 /= 255;   //Convert the blue value to anumber between 0 and 1 (currently is 0 and 255).

  var cMax = Math.max(r2, g2, b2);    // Set cMax to the largest value between
  var cMin = Math.min(r2, g2, b2);    // Set cMain to the smallest value between
  var delta = cMax - cMin;            // Get the difference.

  // Calculate the hue, saturation, and lightness.
  var hsl = {h:0, s:0, l:0} // Create an hsl object to return the hsl color.
  // h is the hue. The default hue is zero (red). It has units of degrees.
  // s is the saturation.The default saturation is zero (none, gray). It is a percentage between 0 and 100.
  // l is the lightness. It is a percentage between 0 and 100.
  hsl.l = (cMax + cMin) / 2;          // The formula for lightness. (still not a percentage here)
  if(delta != 0) {                    // If delta is zero, the color is some shade of gray (between blak and white).
      if(cMax == r2) {                // Is the color is predominately red?
          hsl.h = 60 * (((g2 - b2) / delta ) % 6);  // Yes, calculate the hue.
      } else if(cMax == g2) {         // Is the color is predominately green?
          hsl.h = 60 * (((b2 - r2) / delta ) + 2);  // Yes, calculate the hue.
      } else {                        // The color is predominately blue.
          hsl.h = 60 * (((r2 - g2) / delta ) + 4);  // Yes, calculate the hue.
      }                               // The hue has been calculated.
      hsl.s = delta / (1 - Math.abs(2 * hsl.l - 1)) * 100;// The formula for the saturation.
  }                                   // Still need to convert the lightness to a percentage.
  hsl.l *= 100;                       // Convert the lightness to a percentage.
  var jsonHSL = JSON.stringify(hsl);  // Convert the three hsl numbers (hsl object) into a JSON string to return.
  return jsonHSL;                     // Returns a JSON version of the hsl color.
} // End of function hexToHsl.

function submitFunction() {           // The user wants to submit chnl configurations to the PagesController.
  // Put the chnl configurations into an element to tranfer to the PagesController.
  document.getElementById("theChnls").value = JSON.stringify(theChnls);
  document.getElementById("theBeds").value = JSON.stringify(bedConfigs);
  document.getElementById("peLS").value = JSON.stringify(peLS);
  /*
  var actionValue = document.getElementById("action");// Get access to the element containg the action.
  if (actionValue.value == "none") {  // Has the action been set?
    actionValue.value = "save";       // No. Set it to save.
  } // End of the condition to set an action.
  */
}

document.getElementById("theWarningBtn").onclick = function() {// The user accepted the warning.
  var warningDlg = document.getElementById("warningModal");// Get acces to the warning dialog.
  warningDlg.style.display = "none";  // Remove the warning dialog.
  pageReset();                        // Reset the page.
}

function pageReset() {
  console.log("Doing a page reset.");
  // Remove the chnl table from its parent.
  document.getElementById("chnlTable").removeChild(document.getElementById("theChnlTable"));
  setSaveBtn();                       // Display the save & cancel btns if necessary.
  drawTable();                        // Recreate the chnl table.
}

function setSaveBtn() {
  console.log("Displaying the save/cancel buttons.");
  var theSaveBtn = document.getElementById("saveBtn");// The save button might need to be shown.
  var theCancelBtn = document.getElementById("cancelBtn");// The cancel button might need to be shown.
  var theAction = document.getElementById("action").value;
  // Have there been configuration changes or has there been an update when the save btn is displayed?
  if(isConfigChange == true || theAction == "UpdateSave") {
    theSaveBtn.className = "PEBtnShow PEBtns";// Yes. Show the Save button.
    theCancelBtn.className = "PEBtnShow PEBtns";// Show the Cancel button.
  } else {                            // No. There have been no configuration changes or update with save btn.
    theSaveBtn.className = "PEBtnHide PEBtns";// Hide the Save button.
    theCancelBtn.className = "PEBtnHide PEBtns";// Hide the Cancel button.
  } // End of condition to show the save and cancel button.
}

function submitForm() {               // Submit the form to the server.
  // Convert the chnls object into a JSON string.
  document.getElementById("theChnls").value = JSON.stringify(theChnls);
  // Convert the bed configuration object into a JSON string.
  document.getElementById("theBeds").value = JSON.stringify(bedConfigs);
  // Convert the session configuration object into a JSON string.
  document.getElementById("peLS").value = JSON.stringify(peLS);
  document.getElementById("chnlForm").submit();// Submit the form to the server (PagesController).
}

function updateTime() {
  var theAction = document.getElementById("action").value// Get the previous action.
  // If the count goes over 240, the server will use the GET to display the page.
  var count = 0;                      // Set the default count to zero.
  if(theAction.substr(0, 6) == "Update") {// Was the previous action an update?
    count = Number(theAction.substr(6));// Yes. Get the number of updates.
    count++;                          // Increment the number of updates.
  } // End of condition: the prvious action was an update.
  document.getElementById("action").value = "Update" + count.toString();// Pass the 'Update' command to the controller.
  if(document.getElementById("saveBtn").className == "PEBtnShow PEBtns") {// Is the save btn displayed?
    document.getElementById("action").value = "UpdateSave";// Pass the 'UpdateSave' command to the controller.
  }
  if(document.getElementById("warningOKModal").style.display == "block") {// Is the stop dialog displayed?
    return;                   // Yes. Don't update the page. Updating the page removes the stop dialog.
  } // End of condition: The stop dialog is displayed.
  if(document.getElementById("chnlColorModal").style.display == "block") {// Is the color dialog displayed?
    return;                   // Yes. Don't update the page. Updating the page removes the color dialog.
  } // End of condition: The color dialog is displayed.
  if(document.getElementById("chnlNameModal").style.display == "block") {// Is the name dialog displayed?
    return;                   // Yes. Don't update the page. Updating the page removes the name dialog.
  } // End of condition: The name dialog is displayed.
  if(document.getElementById("chnlInputModal").style.display == "block") {// Is the input dialog displayed?
    return;                   // Yes. Don't update the page. Updating the page removes the input dialog.
  } // End of condition: The input dialog is displayed.
  if(document.getElementById("chnlRelayModal").style.display == "block") {// Is the relay dialog displayed?
    return;                   // Yes. Don't update the page. Updating the page removes the relay dialog.
  } // End of condition: The relay dialog is displayed.
  if(document.getElementById("chnlDeviationModal").style.display == "block") {// Is the deviation dialog displayed?
    return;                   // Yes. Don't update the page. Updating the page removes the deviation dialog.
  } // End of condition: The deviation dialog is displayed.
  if(document.getElementById("chnlAppLimitModal").style.display == "block") {// Is the approach limit dialog displayed?
    return;                   // Yes. Don't update the page. Updating the page removes the approach limit dialog.
  } // End of condition: The approach limit dialog is displayed.
  if(document.getElementById("chnlAddModal").style.display == "block") {// Is the add chnl dialog displayed?
    return;                   // Yes. Don't update the page. Updating the page removes the add chnl dialog.
  } // End of condition: The add chnl dialog is displayed.
  if(document.getElementById("chnlRemoveModal").style.display == "block") {// Is the remove chnl dialog displayed?
    return;                   // Yes. Don't update the page. Updating the page removes the remove chnl dialog.
  } // End of condition: The remove chnl dialog is displayed.
  if(document.getElementById("warningModal").style.display == "block") {// Is the warning dialog displayed?
    return;                   // Yes. Don't update the page. Updating the page removes the warning dialog.
  } // End of condition: The warning dialog is displayed.
  if(document.getElementById("warningStorageModal").style.display == "block") {// Is the storage warning dialog displayed?
    return;                   // Yes. Don't update the page. Updating the page removes the storage warning dialog.
  } // End of condition: The storage warning dialog is displayed.
  if(document.getElementById("waitFor").style.display == "block") {// Is the wait dialog displayed?
    return;                   // Yes. Don't update the page. Updating the page removes the wait dialog.
  } // End of condition: The wait dialog is displayed.
  if(document.getElementById("warningSaveModal").style.display == "block") {// Is the save dialog displayed?
    return;                   // Yes. Don't update the page. Updating the page removes the save dialog.
  } // End of condition: The save dialog is displayed.
  submitForm();               // Submit the form to the server.
}

window.onload=drawTable;      // When the page loads, check for local storage and draw the channels table.
window.setInterval(updateTime, 15000);// Update the Channels page once every 15 seconds.

</script>

@endsection
