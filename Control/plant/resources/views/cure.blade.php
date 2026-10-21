@extends('master')
@section('title', 'Set a Cure Cycle')

@section('content')

<head>

  <!-- http-equiv="refresh" content="1800;url=/cure" causes the page to be
      refreshed every 1800 seconds (30 minutes) and to use a GET instead of a POST. -->
  <meta charset="UTF-8" http-equiv="refresh" content="1800;url=/cure">

  <style type="text/css">
    .dropdown {
        position: relative;
        display: inline-block;
        margin: 0px 0px;
        border-style: solid;
        border-width: thin;
        border-color: gray;
        border-radius: 4px;
        border-type: groove;
        padding: 1px 1px;
        background-color: #f1f1f1;
    }

    .dropdown-content {
      display: none;
      position: absolute;
      min-width: 160px;
      box-shadow: 0px 8px 16px 0px rgba(0,0,0,0.2);
      padding: 12px 16px;
      border-style: solid;
      border-width: thin;
      border-color: gray;
      border-radius: 4px;
      z-index: 1;
    }

    .dropdown:hover .dropdown-content {
        display: block;
    }

    .dropdown:hover {background-color: #56e35c;}


    .prgTable {
      position: relative;
      float: right;
    }

    td {
      text-align: center;
    }

    th {
      padding: 2px 10px;
    }

    #startTimeData {
      position: relative;
      float: right;
      margin-top: 2px;
      margin-right: 2px;
    }

    #bedChnlBtn {
      margin-left: 4px;
      margin-right: 0px;
      width: 80px;
      border-radius: 4px;
      border-color: gray;
      border-width: thin;
      background-color: #f1f1f1;
      padding: 4px;
      margin-top: 4px;
    }

    #bedChnlBtn:hover {background-color: #56e35c;}


    #prgType {
      width: 75px;
      height: 32px;
      font-size: 14px;
      border-radius: 4px;
      border-color: gray;
      border-color: gray;
      border-width: thin;
      background-color: #f1f1f1;
      padding: 4px;
    }

    .PE_CureBtns {
      width: 55px;
      border-radius: 4px;
      border-color: gray;
      border-style: groove;
      border-width: thin;
      background-color: #f1f1f1;
      padding: 4px;
      margin-top: 4px;
    }

    .PE_CureBtns:hover {background-color: #56e35c;}

    #chnlCtrlHeader,
    #refHeader {
      min-width: 55px;
      /*height: 20px;*/
      text-align: center;
      border-radius: 4px;
      border-color: gray;
      padding: 2px;
      margin-top: 2px;
      margin-bottom: 2px;
    }

    #chnlSelect,
    #refSelect {
      height: 200px; overflow: auto; width: 100px; border: 1px solid #000;
      list-style-type: none; margin: 0; padding: 0; overflow-x: hidden;
      float:right;
    }

    /*#chnlCtrlHeader:hover, #refHeader:hover {background-color: #56e35c;}*/

    #cutoffTxt,
    #mainTTxt {
      width: 60px;
      border: 1px solid;
      border-radius: 4px;
      border-color: gray;
      text-align: center;
      margin-right: 8px;
    }

    #cutoffData {
      display: none;
      border: 1px solid;
      border-radius: 4px;
      border-color: lightgray;
      position: relative;
      padding: 2px;
      margin-left: 10px;
    }

    li{
      list-style-type: none;
    }

    #refDisplay {
      display: none;
      position: relative;
      float: left;
      padding-left: 15px;
    }

    #start_time {
      border-radius: 4px;
      border-color: gray;
      border-style: groove;
      border-width: thin;
      background-color: #f1f1f1;
      padding: 4px;
      margin-top: 2px;
    }

    #start_time:hover {background-color: #56e35c;}

    .editButton {
      width: 150px;
      border-radius: 4px;
      padding: 4px;
      margin: 10px;
    }

    label { display: block; color: WindowText; background-color: Window; margin: 0; padding: 0px 0px 0px 2px; width: 100%; }
    label:hover { background-color: #CBF6D2; color: HighlightText; }

    /* The Modal (background) */
    .PEmodal {
      display: none; /* Hidden by default */
      position: fixed; /* Stay in place */
      z-index: 1; /* Sit on top */
      padding-top: 25px; /* Location of the box */
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
      width: 30%;
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
<form id="cureForm" method="post">

  <!-- Display any errors. -->
  @foreach ($errors->all() as $error)
    <p class="alert alert-danger">{{ $error }}</p>
  @endforeach

  @foreach ($scErrors as $error)
    <p class="alert alert-danger">{{ $error }}</p>
  @endforeach

  <!-- The next line submits the token required by Laravel -->
  <input type="hidden" name="_token" value="{!! csrf_token() !!}">
  <!-- Create a hidden input to hold the name of the item being programmed. -->
  <input type="hidden" id="prgDex" name="prgDex" value="{!! $prgDex !!}">
  <!-- Create a hidden input to hold the program. -->
  <input type="hidden" id="thePrg" name="thePrg">
  <!-- Create a hidden input to hold the program points. -->
  <input type="hidden" id="thePrgPoints" name="thePrgPoints">
  <!-- Create a hidden input to hold the action to be performed during an update. -->
  <input type="hidden" id="cureAction" name="cureAction" value='none'>
  <!-- Create a hidden input to hold whether a bed or chnl is being programmed. -->
  <input type="hidden" id="what" name="what" value="{!! $what !!}">
  <!-- Create a hidden input to hold the action to be performed during a bed update. -->
  <input type="hidden" id="theBeds" name="theBeds">
  <!-- Create a hidden input to hold the session configuration. -->
  <input type="hidden" id="peLS" name="peLS">

  <span id=startTimeData>Start Time: <input type="text" id="start_time" name="start_time"></span>

<!-- Create a button that toggles between Bed and Channel so that a user can program either a bed or a chnl. -->
  <input type="button" id="bedChnlBtn" onclick="toggleBedChnl()">
  <div class="dropdown">
    <!-- The "Channels" drop down list label never appears unless there are no channels.
          The chnlCtrlHeader is required to display the channel that is selected. -->
    <div id="chnlCtrlHeader">Channels</div>
    <ul class="dropdown-content" id="chnlSelect"></ul>
  </div>

  <select id="prgType" onchange="changeType()">
    <option id="typeParent" value="parent">parent</option>
    <option id="typeChild" value="child">child</option>
    <option id="typeCutoff" value="cutoff">cutoff</option>
    <option id="typeMimic" value="mimic">mimic</option>
  </select>
  <input type="button" class="PE_CureBtns" value="New" onclick="onNewPrg()">
  <input type="button" class="PE_CureBtns" value="Start" onclick="onStartPrg()">
  <input type="button" class="PE_CureBtns" id="stopBtn" value="Stop" onclick="onStopPrg()">
  <input type="button" class="PE_CureBtns" value="Save" onclick="onSavePrg()">
  <input class="PE_CureBtns" id="submitBtn" type="submit" value="Update">

  <!-- Create a drop down list which allows the user to select a reference channel. -->
  <p></p>
  <div id="refDisplay">
    Reference:
    <div class="dropdown">
      <div id="refHeader">Channels</div>
      <ul class="dropdown-content" id="refSelect"></ul>
    </div>
  </div>

  <div id="cutoffData">
    Cutoff Temp:
    <input id="cutoffTxt" type="text">
    Maintenance Temp:
    <input id="mainTTxt" type="text">
  </div>

  <!-- Create the program table -->
  <div class="prgTable">
    <table border="0" width="300" onkeydown="tableKeyDown(event)">
      <tr>
        <td>
          <table border="1" width="300" >
            <tr style="color:white;background-color:grey">
              <th>Point</th>
              <th>Time</th>
              <th>Temp</th>
              <th>PR#</th>
            </tr>
          </table>
        </td>
      </tr>
      <tr>
        <td>
          <div style="width:312px; height:96px; overflow:auto;">
            <table id="prg_point_table" border="1" width="300">
            </table>
          </div>
        </td>
      </tr>
    </table>
  </div>

</form>

<h2 style="text-align:center;">Set a Cure Cycle</h2>
<canvas id="myPrgChart" width=400px height=200px></canvas>

<!-- Modal dialog to edit program points. -->
<div id="editPrg" class="PEmodal">
  <!-- Modal content-->
  <div class="PEmodal-content">
    <span id="closeEdit" class="PEclose">&times;</span>
    <h4>Edit:</h4>
    <div id="edit_form">
      <p></p>
      <button id="insertPntBtn" class="editButton" type="button">Insert a Point before </button>
      <button id="deletePntBtn" class="editButton" type="button">Delete the Point </button>
      <button id="addPntBtn" class="editButton" type="button">Add Point to End</button>
      <button id="cancelPntBtn" class="editButton" type="button">Cancel</button>
    </div>
  </div>
</div>

<!-- Modal warning that the Plant manager is being communicated with. There will be a delay. -->
<div id="waitFor" class="PEmodal">
  <!-- Modal content-->
  <div class="PEmodal-content">
    <h2 style="text-align:center;">Talking to the Plant Manager.</h2>
    <h2 style="text-align:center;">Please Wait.</h2>
  </div>
</div>

<!-- The modal dialog to assign a bed chnl. -->
<div id="assignChnl" class="PEmodal">
  <!-- modal content -->
  <div class="PEmodal-content" style="width:50%;">
    <span id="closeAssignChnl" class="PEclose">&times;</span>
    <h4><br>Assign a system channel to bed channel
    <span id="bedChnlNumber">number</span>
    <div id="selectAssignedChnl" style="padding: 10px"></div>
    </h4>
    <button id="setAssignedChnlBtn" class="bedBtns" style="margin-left:38px">Select</button>
    <button id="cancelSetAssignedChnlBtn" class="bedBtns">Cancel</button>
  </div>
</div><!-- End of the dialog to assign a bed chnl. -->

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
var scChnls = <?php echo json_encode($chnls); ?>;// Unpack the scChnl configurations. Required by js/cureChart.js.
var bedConfigs = <?php echo json_encode($theBeds); ?>;// Unpack the bed configurations.
var bedConfigsCopy = bedConfigs;      // Create a backup in case the user cancels assigning chnls.
var prgDex = document.getElementById("prgDex").value;// The scChnlDex of the chnl or bed chnl being programmed.
var prgData = <?php echo json_encode($prgData); ?>;// Unpack the program data.
var prgChart = <?php echo json_encode($prgChart); ?>;// Unpack the program data.
var peLS = <?php echo json_encode($peLS); ?>;// Contains this session's configuration.
var theEditDex = -1;                  // The index of the point being modified. -1 means no points are being modified.
var prgWhat = document.getElementById("what").value;// Indicates if a Bed or Chnl is being programmed.
// Parse out the index of the bed being programmed.
var bedDex = prgWhat.substr(prgWhat.indexOf("-") + 1, prgWhat.length - prgWhat.indexOf("-") - 1);
bedDex = Number(bedDex);              // Make sure the bed's index is a number (not a string).
prgWhat = prgWhat.substr(0, prgWhat.indexOf("-"));// Parse out what is being programmed (Bed or Chnl).
var chartLabel = "Program for channel " + scChnls[prgDex].name;// Required by js/cureChart.js.
if(prgWhat == "Bed") {                // Is a bed being programmed?
  if(bedConfigs['bed' + bedDex]['bedChnl0']['scChnlDex'] == -1) {// Yes. Does it have an assigned 1st chnl?
    if(bedConfigs['bed' + bedDex]['bedChnl0']['assignedChnlDex'] == -1) {// Yes. Has the 1st chnl been assigned?
      chartLabel = "Assigned to channel none";// No.
    } else {                          // Change the chart label to show the assignment.
      chartLabel = "Assigned to channel " + scChnls[bedConfigs['bed' + bedDex]['bedChnl0']['assignedChnlDex']].name;
    } // End of setting the chart's label.
  } // End of modifying the chart label for a bed's 1st chnl which was assigned.
  document.getElementById("prgType").style.display = "none";// Hide the edit program dialog.
  document.getElementById("refDisplay").style.display = "none";// A bed doesn't has a reference. Don't Display a reference.
} else {                              // A chnl is being programmed.
  // Regardless of the program type, set the cutoff and maintenance tmeps for this chnl. The user might change to cutoff.
  document.getElementById("cutoffTxt").value = prgData[0].cutoffTemp;// Set the cutoff.
  document.getElementById("mainTTxt").value = prgData[0].maintenanceTemp;// Set the maintenance.
  switch (Number(prgData[0].type)) {  // Show the correct selected control type.
    case 1:                           // A parent.
      document.getElementById("typeParent").selected = true;// Select a parent.
      break;                          // Parent selected.
    case 2:                           // A Child.
      document.getElementById("typeChild").selected = true;// Select a child.
      document.getElementById("refDisplay").style.display = "block";// A child has a reference. Display its reference.
      break;                          // Child selected.
    case 3:                           // A cutoff.
      document.getElementById("typeCutoff").selected = true;// Select a cutoff.
      document.getElementById("refDisplay").style.display = "block";// A cutoff` has a reference. Display its reference.
      document.getElementById("cutoffData").style.display = "inline-block";
      break;                          // Cutoff selected.
    default:                          // A mimic.
      chartLabel = "No " + chartLabel;// There is no program for a mimic.
      document.getElementById("typeMimic").selected = true;// Select a mimic.
      document.getElementById("refDisplay").style.display = "block";// A mimic has a reference. Display its reference.
      break;                          // Mimic selected.
  } // End of switch based on program type.
} // End of set up for a "Bed" or "Chnl".
var startBedChnlDex = 0;              // The bed chnl index of the first bed chnl to analyze for assigment.
                                      // Changes as each assigned bed chnl is assigned. Is returned to zero
                                      // before assigning bed chnls again.
///////////////End/Global/Variables//////////////////////////////////////////

// This function creates a list of channels a user can select from.
// This function cannot be called until the elements with ids "headerId" and "listId" have been created.
function createChnlList(headerId, listId) {// Create a list of all the scChnls.
  if(prgWhat == "Bed") {              // Is a bed being programmed?
    if(listId == "refSelect") {       // Yes. Is a reference list attempting to build?
      return;                         // Yes. A bed does not have a reference list. Discontinue.
    }                                 // The list of beds is to be created.
    createBedList(headerId, listId);  // Create the list of beds.
    return;                           // The list has been created.
  }                                   // A list of chnls or references should be created.
  var chnlList = document.getElementById(listId);// Get access to the list.
  var refDex = 9999;                  // Set the reference's index to invalid.
  if(listId == "refSelect") {         // Is this list to choose a reference?
    refDex = prgData[0].refChnlDex;   // Yes. Set the refDex to the zero based reference's index.
    if(refDex < 0) {                  // Does this chnl have a reference?
      return;                         // No. There is no need to build a reference list.
    } // End of condition to build a reference list if a refDex exists.
  } // End of condition to build a reference chnl list.
  var i = 0;                          // Start with the first chnl index (zero based).
  for(; i < scChnls.length; i++) {   // Go through all the chnl indexes
    if(listId == "refSelect" && prgDex == i) {// Is this list to select a reference & is this chnl the program chnl.
      continue;                       // A chnl cannot be referenced to itself.
    }                                 // This is a chnl that can appear on the list.
    var chnlItem = document.createElement("li");// Create a list item for this chnl.
    chnlItem.addEventListener("click", changeChnl);// Add an chnl change event to this list item (chnl).
    var called = document.createTextNode(scChnls[i].name);// Set this chnl's list item to this chnl's name.
    var myLabel = document.createElement("label");// Create a label for this chnl's list item.
    myLabel.setAttribute("id", "chnl" + i);// Give the label an id for access elsewhere.
    //myLabel.style.backgroundColor = "lightgray";// Give the label a light gray back ground.
    if(prgDex == i || refDex == i) {  // Is this the selected chnl?
      myLabel.style.backgroundColor = "56e35c";// Yes. Set its background to white.
    }                                 //
    myLabel.style.color = peLS['PE_Chnl_Color'][i];// Set the label's color (text color) to the chnl's color.
    myLabel.appendChild(called);      // Set the label text to the chnl's name.
    chnlItem.appendChild(myLabel);    // Put the label in this chnl's list item.
    chnlList.appendChild(chnlItem);   // Put this chnl's list item in the list of chnls.
  }                                   // The chnl list is complete.
  var chnlTitle = document.getElementById(headerId);// Get access to the list's header.
  var theDex = prgDex;                // The default chnl index to display is for the program chnl.
  if(listId == "refSelect") {         // Is this list to select a reference?
    if(refDex < scChnls.length){     // Yes. Does the refence index refer to a valid chnl?
      theDex = refDex;                // Yes. The chnl index to display is for the reference.
    } else {                          // A valid reference has not been selected.
      chnlTitle.innerHTML = "Set Reference";// Prompt the user to select a reference.
      return;                         // There is no chnl to set at the top of the list.
    }                                 // The select a reference prompt has been set.
  }                                   // The index is set.
  console.log("theDex = " + theDex);
  chnlTitle.innerHTML = scChnls[theDex].name;// Display the currently selected chnl.
  chnlTitle.style.fontWeight = "900"; // Make it bold.
  chnlTitle.style.color = peLS['PE_Chnl_Color'][theDex];    // Set its color.
} // End of function createChnlList().

function createBedList(headerId, listId) {// Create a list of all the beds.
  var bedList = document.getElementById(listId);// Get access to the list.
  var bDex = 0;                       // Start with the first bed (zero based).
  for(; bDex < bedConfigs.noBeds; bDex++) {// Go through all the bed indexes.
    var bedItem = document.createElement("li");// Create a list item for this bed.
    //Starat here, use the global bedDex to highlight the selected bed.
    var bTxt = "bed" + bDex;          // Create the key to this bed object in bedConfigs.
    bedItem.addEventListener("click", changeBed);// Add an bed change event to this list item (bed).
    var called = document.createTextNode(bedConfigs[bTxt].name);// Set this bed's list item to this bed's name.
    var myLabel = document.createElement("label");// Create a label for this bed's list item.
    myLabel.setAttribute("id", "bed" + bDex);// Give the label an id for access elsewhere.
    //myLabel.style.backgroundColor = "#f1f1f1";// Give the label a light gray back ground.
    if(bedDex == bDex) {              // Is this the selected bed?
      myLabel.style.backgroundColor = "#56e35c";// Yes. Set its background to white.
    }                                 //
    myLabel.appendChild(called);      // Set the label text to the bed's name.
    bedItem.appendChild(myLabel);     // Put the label in this bed's list item.
    bedList.appendChild(bedItem);     // Put this bed's list item in the list of beds.
  }                                   // End of the loop to go through all the beds.
  var bedTitle = document.getElementById(headerId);// Get access to the list's header.
  var bTxt = "bed" + bedDex;          // Create the key to this bed object in bedConfigs.
  bedTitle.innerHTML = bedConfigs[bTxt].name;// Display the currently selected bed.
  bedTitle.style.fontWeight = "900"; // Make it bold.
} // End of function createBedList().

// This function responds to a request to select a different channel or reference chnl.
// This functtion cannot be called until the elements "cureAction", and "submitBtn" have been created.
function changeChnl(event) {
  var chnlName = event.target.innerHTML;// Get the name of the selected chnl.
  var chnlTitle = event.target;       // Get the label associated with the selected list item.
  var parentId = chnlTitle.parentElement.parentElement.id;// Get the id of the selector with the changed chnl.
  // The id allows us to know if a chnl is being changed or a reference is being changed.
  for(var dex = 0; dex < scChnls.length; dex++) {// Go through each chnl.
    document.getElementById("chnl" + dex).style.backgroundColor = "lightgray";// Set this list item's background to default.
    if(scChnls[dex].name == chnlName) {// Is this the selected chnl?
      // Yes. We want to get the selected chnl's color.
      chnlTitle.style.color = peLS['PE_Chnl_Color'][dex];// Set the list item's text color.
      document.getElementById("chnl" + dex).style.backgroundColor = "white";// Set the background color to show it is selected.
      chnlTitle.style.fontWeight = "900";// Make the font very bold.
      if(parentId == "chnlSelect") {  // Is a new chnl being selected rather than a new reference?
        document.getElementById("prgDex").value = dex;// A new chnl is being selected.
      } else {                        // A new reference is being selected.
        prgData[0].refChnlDex = dex;  // Set the reference's scChnlDex.
        var refTitle = document.getElementById("refHeader");// Get the div containing the reference selector.
        refTitle.innerHTML = chnlName;// Adust the display to show the channel's name,
        refTitle.style.color = colorTxt;// and its color,
        refTitle.style.fontWeight = "900";// and a very bold font.
      }                               // The selected chnl has been set to display.
    } // End of if this chnl was the selected chnl.
  } // End of going through all the chnls.
  if(parentId == "chnlSelect") {      // Is a new chnl being selected rather than a new reference?
    document.getElementById("cureAction").value = 'chnlChange';// A new chnl. Tell the controller so it can get a new program.
    document.getElementById("submitBtn").click();// Cause a post to open the controller to display a new chnl.
  }   // The cureAction has been set.
} // End of function changeChnl().

function changeBed(event) {           // The user wants to change the bed.
  var bedName = event.target.innerHTML;// Get the new bed name.
  var bedTitle = event.target;        // Get access to the bed list's Title.
  var parentId = bedTitle.parentElement.parentElement.id;
  bedTitle.innerHTML = "Bed " + bedName;// Set the bed list's title to the new bed's name.
  var bDex = 0;                       // Start with the first bed index (zero based).
  for(; bDex < bedConfigs.noBeds; bDex++) {// Go through all the bed indexes.
    var bKey = "bed" + bDex;          // Create this bed's object key.
    if(bedConfigs[bKey].name == bedName) {// Is this bed's name the same as the selected bed?
      break;                          // Yes. Found the bed index for the new bed. No need to look farther.
    }                                 // This bed's name has been compared to the selected bed's name.
  }                                   //
  document.getElementById("what").value = "Bed-" + bDex;// Pass the bed index of the new bed selection.
  document.getElementById("cureAction").value = 'bedChange';// Tell the controller so it can get a new program for the new bed.
  document.getElementById("submitBtn").click();// Cause a post to open the controller.
}

// !!!!
function changeStartTime() {
  document.getElementById("submitBtn").click();// Cause a post to open the controller.
}
// !!!!

function createPointTable() {
  console.log("createPointTable");
  for(var dex = 0; dex < prgChart.length; dex++) {// Go through each program point.
    var prg_row = document.createElement("tr");// Create a row for the point.
    var prg_point = document.createElement("td");// Create a cell to contain the point's designation.
    prg_point.setAttribute("style", "width: 70px");// Set attributes for the point's designation.
    var prg_point_btn = document.createElement("button");// Create a button for the point designation.
    prg_point_btn.setAttribute("style", "width: 100%");// Set its attributes.
    prg_point_btn.setAttribute("type", "button");// Prevents this button from sending a submit.
    prg_point_btn.addEventListener("click", function() {// Add an event listener to the point button.
      pointBtnClicked(this);            // This allows the progrma points to be modified.
    });                                 // The point button listener has been added.
    if(dex == 0) {                      // Is this the start point?
      prg_point_btn.innerHTML = "Start";// Yes. Display start rather than zero.
    } else {                            // This is not the start point.
      prg_point_btn.innerHTML = dex;    // Display the points number.
    } // End of displaying the point's designation.
    var prg_time = document.createElement("td");// Create a cell for this program point's time.
    prg_time.setAttribute("style", "width: 70px");// Set attributes for the cell.
    if(dex != 0) {                      // Is this point something other than the start point?
      prg_time.setAttribute("contenteditable", "true");// Yes. Allow its time to be changed.
    } // End of allowing times other than the start time to be changed.
    var elapsed_time_txt = timeTxt(prgChart[dex].x);// Create a string containging the elapsed hours and minutes.
    var prg_time_txt = document.createTextNode(elapsed_time_txt);// Create text for the time cell.
    var prg_temp = document.createElement("td");// Create a cell for this program point's temperature.
    prg_temp.setAttribute("contenteditable", "true");// Allow the user to edit the temperature in the cell.
    prg_temp.setAttribute("style", "width: 70px");// Set the attributes for the cell.
    var temp_number = prgChart[dex].y;        // Get this point's temperature (it is a string).
    var aNumber = parseFloat(temp_number);    // Convert the tempearture to a float number.
    var some_temp_txt = aNumber.toFixed(1);   // Create a temperature string with one decimal point.
    var prg_temp_txt = document.createTextNode(some_temp_txt);// Create a text node for this point's temperature.
    var prg_pr_no = document.createElement("td");// Create a cell for this program point's priority number.
    var prg_pr_no_txt = document.createTextNode("---------");// Start here, allow the user to enter priority numbers.
    prg_point.appendChild(prg_point_btn);// Make the program point button part of the point cell.
    prg_time.appendChild(prg_time_txt); // Make the time text part of this program point's time cell.
    prg_temp.appendChild(prg_temp_txt); // Make the temperature text part of this program point's tmperature cell.
    prg_pr_no.appendChild(prg_pr_no_txt);// Make the priority number text part of this program point's priority# cell.
    prg_row.appendChild(prg_point);     // Make the program point cell part of this row.
    prg_row.appendChild(prg_time);      // Make the program time cell part of this row.
    prg_row.appendChild(prg_temp);      // Make the program temperature cell part of this row.
    prg_row.appendChild(prg_pr_no);     // Make the program priority number cell part of this row.
    prg_point_table.appendChild(prg_row);// Make this row part of the table.
  } // End of loop adding rows for each progrma point.
} // End of function createPointTable().

function timeTxt(hours) {             // Create a string representation given elapsed hours in the form of hours & minutes.
  var theHours = Math.floor(hours);   // Round the given hours down to the nearest integer.
  var hours_txt = theHours.toString();// Convert the rounded hours to a string.
  if(theHours < 10) {                 // Are the hours a single digit?
    hours_txt = "0" + hours_txt;      // Yes. Put a zero in fron of the hours so there are at least twp hours digits.
  } // End of assuring there are two hours digits.
  var theMinutes = Math.round((hours - theHours) * 60); // Calculate the whole minutes of the hour.
  var minutes_txt = theMinutes.toString();// Convert the rounded minutes to a string.
  if(theMinutes < 10) {               // Are the minutes a single digit?
    minutes_txt = "0" + minutes_txt;  // Yes. Put a zero in fron of the minutes so there are at least twp minutes digits.
  } // End of assuring there are two minutes digits.
  return hours_txt + ":" + minutes_txt;// Create a string to display hours, a colon, and minutes.
} // End of function timeTxt().

function pointBtnClicked(btnObj) {
  // The user clicked a button in the point column.
  // We need to determine which point button he clicked.
  var dex = btnObj.innerHTML; // Using the button object, get the point name.
  if(dex == "Start") {        // Was the name for the start point?
    theEditDex = 0;           // Yes. The start point has an index of zero.
  } else {                    // All the other points have names which
    theEditDex = parseInt(dex);// correspond to their index.
  }                           // The point's index has been set.
  if(theEditDex < 1) {        // Was the Start point selected?
    // Yes. The user is not allowed to delete the Start point.
    document.getElementById("deletePntBtn").style.display = "none";
    // The user is not allowed to insert a point before the Start point.
    document.getElementById("insertPntBtn").style.display = "none";
  } else {                    // The user selected a point other than the Start point.
    document.getElementById("deletePntBtn").style.display = "inline-block";
    if(prgChart.length >= 21) {
      // The user is not allowed to insert a point if the chart already has 21 points.
      document.getElementById("insertPntBtn").style.display = "none";
      // The user is not allowed to insert a point if the chart already has 21 points.
      document.getElementById("addPntBtn").style.display = "none";
    } else {
      document.getElementById("insertPntBtn").style.display = "inline-block";
    }
  }                           // It is OK delete any point but the Start point.
  document.getElementById("insertPntBtn").innerHTML = "Insert a Point before " + dex;
  document.getElementById("deletePntBtn").innerHTML = "Delete the Point " + dex;
  document.getElementById("addPntBtn").innerHTML = "Add Point to End";
  document.getElementById("editPrg").style.display = "block";// Display the modal dialog to edit a prg point.
} // Done setting the index of the point to be edited.

// Add an event to respond to the user clicking on something which requires posting data to the PagesController.
document.getElementById("cureForm").addEventListener("submit", submitClicked);

function submitClicked() {            // The user clicked on something which requires posting data to the PagesController.
  var usTime = document.getElementById("start_time").value;// The start time element is in US date format.
  var isoTime = usTime.substr(6, 4) + "-";// Start with the year.
  isoTime += usTime.substr(0, 2) + "-";   // Add the month.
  isoTime += usTime.substr(3, 2) + " ";   // Add the day.
  isoTime += usTime.substr(11, 2) + ":";  // Add the hour.
  isoTime += usTime.substr(14, 2);        // Add the minute.
  prgData[0].timeToStart = isoTime;       // Set the possible new start time.
  if(prgWhat != "Bed") {              // Is a Bed currently being programmed?
    var ctrlTxt = ["----", "parent", "child", "cutoff", "mimic", "cylinder"];// No. These are the possible control types.
    var theType = document.getElementById("prgType").value;// Get the user's selected control type?
    for(var tDex = 1; tDex < ctrlTxt.length; tDex++) {// Go through each control type.
      if(theType == ctrlTxt[tDex]) {  // Is this the selected control type?
        prgData[0].type = tDex;       // Yes. Set the chnl's control type.
        if(ctrlTxt[tDex] == "cutoff") {// Is the control type cutoff?
          prgData[0].cutoffTemp = document.getElementById("cutoffTxt").value;// Yes. Save the cutoff temperature.
          prgData[0].maintenanceTemp = document.getElementById("mainTTxt").value;// Also save the maintenanceTemp.
        } // End of processing a cutoff.
        break;                        // No need to check more.
      } // The control type has been set.
    } // All potential control types have been evaluated.
  } // End of programming a child, cutoff, or mimic.
  var j_prgData = JSON.stringify(prgData);// Convert the program object to a JSON string.
  document.getElementById("thePrg").value = j_prgData;// Set the hidden input for the program (Laravel thing).
  var j_bedConfigs = JSON.stringify(bedConfigs);// Convert the bed configurations object into a JSON string.
  document.getElementById("theBeds").value = j_bedConfigs;// Set the hidden input for the bed configs (Laravel thing).
  var thePrgTable = document.getElementById("prg_point_table");// Access the program's points table.
  // Get data from the Start row.
  if(theEditDex == -1) {              // Has the number of points been modified?
    // No. Get the program points from the table.
    for(var pointDex = 0; pointDex < thePrgTable.rows.length; pointDex++) {// Go through each point in the program.
      var rowData = thePrgTable.rows[pointDex].cells;// Get the row for this point.
      // Only do the time and temp columns now. Remove the -1 to do PR#s in the future.
      for(var colDex = 1; colDex < rowData.length - 1; colDex++) {// Go through each column.
        var minutes = 0;              // Contains the number of elapased minutes.
        var temp = 0;                 // Contains the default temperature.
        if(colDex == 1) {             // Is this a time column?
          var timeTxt = rowData.item(colDex).innerHTML;// Get the text which represents the elapsed time.
          minutes = timeTxtToMinutes(timeTxt);// Yes. Convert the time text into elapsed minutes.
          prgChart[pointDex].x = minutes / 60;// Store this point's elapsed hours.
        } else {                      // This is a temperature column.
          var temperatureTxt = rowData.item(colDex).innerHTML;// Get the text representing the temperature.
          if(isNaN(temperatureTxt) == true) {// Does this text convert to a number?
            temp = -100;              // No. Set the program temperature to -100 to indicate an error.
          } else {                    // This text does represent a number.
            temp = parseFloat(temperatureTxt);// Convert the text to a number.
            if(temp < -99) {          // Prevent temperatures below -99.
              temp = -101             // The user entered a number below -99. Set an error flag.
            } else if (temp > 300) {  // Prevent the user from enetering a temperture over 300.
              temp = -102;            // the user entered a number over 300. Set an error flag.
            }                         // The numeric temperature text has been processed.
          }                           // The temperature is ready to be set.
          prgChart[pointDex].y = temp;// Store this point's temperature.
        } // Done processing a temperature column.
      } // Done going through this row's columns.
    } // Done going through this program's points.
  } // Done processing if theEditDex == -1.
  var j_prgPoints = JSON.stringify(prgChart);// Convert the program points object to a JSON string.
  document.getElementById("thePrgPoints").value = j_prgPoints;// Set the hidden input for the program points (Laravel thing).
  if(document.getElementById("cureAction").value == 'setPrg') {// Is communcation with the PM going to happen?
    document.getElementById("waitFor").style.display = "block";// Yes. This takes a while. Alert the user.
  } // Done warning the user.
  // Convert the session configuration object into a JSON string.
  document.getElementById("peLS").value = JSON.stringify(peLS);
} //End of function submitClicked().

function changeType() {               // The user wants to change a chnl's control type.
  document.getElementById("cureAction").value = 'changeType';// Tell the controller the user is changing the control type.
  document.getElementById("submitBtn").click();// Create a sunbit to reload the page.
}

function toggleBedChnl() {            // The user wants to change from programming either a Bed or Chnl to the other.
  if(prgWhat == "Bed") {              // Is a Bed currently being programmed?
    document.getElementById("what").value = "Chnl-0";// Provide the controller what the user is programming. (Chnl => chnl)
  } else {                            // No. A Channel is currently being programmed.
    document.getElementById("what").value = 'Bed-0';// Provide the controller what the user is programming. (Bed => bed).
  }                                   // The Bed/Chnl button has been toggled.
  document.getElementById("cureAction").value = 'toggleBedChnl';// Tell the controller the user is toggling between bed & chnl.
  document.getElementById("submitBtn").click();// Create a sunbit to reload the page.
}

function onNewPrg() {                           // Tell the controller to create a new program.
  document.getElementById("cureAction").value = 'new';// 'new' is the command to create a new program.
  document.getElementById("submitBtn").click(); // Cause the controller to create the new program.
}

function onStartPrg() {               // Tell the controller to have the PM read the new program.
  // If this is for a bed and if the bed has assigned channels. Assign those chnls here.
  console.log("onStartPrg called!");
  document.getElementById("cureAction").value = 'setPrg';// This action will tell the Plant Manager to read this active program.
  if(prgWhat == "Bed") {              // Is a bed being programmed?
    assignBedChnls();                 // Assign any bed chnls that need assigning.
    if(startBedChnlDex != 0) {        // Have all the bed chnl that need assigning been assigned.
      return;                         // Returning here causes the next bed chnl to be assigned.
    } // End of assigning bed chnls.
  } // End of if a bed is being programmed.
  prgData[0].active = 1;              // Make sure the program is active.
  document.getElementById("submitBtn").click();// Cause the controller update the cure cycle using the setPrg action.
} // End of function onStartPrg().


function assignBedChnls() { // Allows the user to assign chnls to a bed.
  var theBed = bedConfigs['bed' + bedDex];// Yes. Access the bed being programmed.
  for(var cDex = startBedChnlDex; cDex < theBed['noChnls']; cDex++ ) {// Go through each bed channel.
    if(theBed['bedChnl' + cDex]['scChnlDex'] == -1) {// Is this bed chnl assigned?
      document.getElementById("bedChnlNumber").innerHTML = (cDex + 1).toString();
      // Yes. Create a chnl list in order to select an scChnl to assign to this bed chnl.
      createAssignmentChnls(theBed['bedChnl' + cDex]['assignedChnlDex']);
      // Display the list of chnls to select one to be assigned to this bed chnl.
      document.getElementById("assignChnl").style.display = "block";
      startBedChnlDex++;            // Increment to evaluate the next bed chnl when this function is called again.
      return;                       // Done until this bed chnl has an scChnl assigned to it.
    } // Done with check if this is an assigned chnl.
  } // Done going through all the bed chnls looking for ones which need to have an scChnl assigned to them.
  // Reset to zero so next time a bed programmed is started, it can be
  startBedChnlDex = 0;              // analyzed for assigned chnls.
}

document.getElementById("setAssignedChnlBtn").onclick = function() {// The user clicked the btn to assign a bed chnl.
  document.getElementById("assignChnl").style.display = "none";// Hide the dialog for selecting an assigned chnl.
  // Modify the assignedChnlDex in bedConfigs for the bed chnl which is configured to
  // be assigned on the bed being programmed.
  var chnlName = document.getElementById("assignedChnlSelector").value;// Get the name of the chnl being assigned to this bed.
  var scChnlDex = chnlNameToSCChnlDex(chnlName);// Get the scChnlDex of the chnl being assigned to the bed.
  if(scChnlDex == -1) {               // Does the chnl name correspond to an actual chnl?
    scChnlDex = 0;                    // No. For now, select the first chnl. Later produce an error message.
  }                                   // A valid scChnlDex has been obtained for the chnl being assigned to this bed.
  var bKey = "bed" + bedDex;          // Create the key to this bed object.
  // Get the bedChnlDex from the page. The page's bedChnlDex has been incremented by one to make it more
  var bedChnlDex = Number(document.getElementById("bedChnlNumber").innerHTML) - 1;// human compatible.
  console.log("The bedChnlDex is " + bedChnlDex);
  var bcKey = "bedChnl" + bedChnlDex; // Create the key to this bed chnl object.
  // Modify assignedChnlDex to contain the selected chnl's scChnlDex.
  bedConfigs[bKey][bcKey]['assignedChnlDex'] = scChnlDex;
  if(document.getElementById("cureAction").value == 'setPrg') {// Is a program being started?
    onStartPrg();                     // Finish starting the program(s).
  } else {                            // The program(s) is being saved.
    onSavePrg();                      // Finsh saving the program(s).
  } // End of directing the flow to the appropriate action (start or save a program(s)).
} // End of function which responds to the user selecting an assigned chnl.

document.getElementById("cancelSetAssignedChnlBtn").onclick = function() {// The user canceled assigning chnls.
  document.getElementById("assignChnl").style.display = "none";// Hide the assign chnl dialog.
  bedConfigs = bedConfigsCopy;        // Restore bedConfigs to its state before the cancel.
  startBedChnlDex = 0;                // Be sure to start with the 1st bed chnl next time assignment is done.
}

function chnlNameToSCChnlDex(name) {  // Provide the caller with the scChnlDex for the chnl with the given name.
  for(var scDex = 0; scDex < scChnls.length; scDex++) {// Go through each chnl on this system.
    var scChnl = scChnls[scDex];     // Get this chnls scChnl configuration.
    if(scChnl.name == name) {         // Is this chnl's name the same as the given name?
      return scDex;                   // Provide the caller with the scChnlDex for the chnl with the given name.
    }                                 // The names didn't match.
  }                                   // All the chnls have been tested for the given name.
  return -1;                          // Provide the caller with an invalid scChnlDex.
} // End of function chnlNameToSCChnlDex().


function createAssignmentChnls(assignedChnlDex) {
  var assignListLoc = document.getElementById("selectAssignedChnl");// Get the div which holds the list of chnls.
  while(assignListLoc.hasChildNodes()) { // Totally clear the assigned chnl selector.
    assignListLoc.removeChild(assignListLoc.firstChild);// Remove this node.
  } // The assigned chnl selector is empty.
  console.log("Empty assigned chnl list.");
  var sel = document.createElement("select");// Create a selector containg all this system's chnls.
  sel.setAttribute("size", 6);      // Create a selectable list which displays 6 selections even though there may be more.
  var bTxt = "bed" + bedDex;        // Create the object key for this bed.
                                    // It is used to select the default chnl.
  // Create a list of system channels.
  var selectedDex = assignedChnlDex;  // Set the index of the selected chnl.
  if(selectedDex == -1) {             // Was the selected index valid?
    selectedDex = 0;                  // No, selected the system's first chnl.
  }                                   // The selected index has been set.
  for(var scDex = 0; scDex < scChnls.length; scDex++) {// Go through each chnl on this system.
    var opt = document.createElement("option");// Create a row in the selector for this system chnl.
    if (scDex == selectedDex) {       // Is this the chnl to show as selected?
      opt.selected = true;            // Yes. Select this chnl as the default.
    }                                 // Chnl selection has been processed.
    var scChnl = scChnls[scDex];        // Get this chnls scChnl configuration.
    var chnlTxt = document.createTextNode(scChnl.name);// Put its name in a text node.
    opt.appendChild(chnlTxt);         // Make the chnl's name a part of this option.
    sel.appendChild(opt);             // Make this option a part of the selector.
  }                                   // The system chnl list has been updated for this SCChnl.
  sel.style.width = "102px";          // Set a uniform width for the dropdown.
  sel.style.borderRadius = "4px";     // Round the corners of the selection box.
  sel.id = "assignedChnlSelector";    // Allow access to the bed channel selector.
  assignListLoc.appendChild(sel);     // Make the selection list a part of the modal dialog.
} // End of function createAssignmentChnls().

function onStopPrg() {                          // Tell the controller to have the PM read the new program.
  prgData[0].active = 0;                        // Make sure the program is inactive.
  document.getElementById("cureAction").value = 'setPrg';// This action will tell the Plant Manager to read this active program.
  document.getElementById("submitBtn").click();// Cause the controller update the cure cycle using the setPrg action.
}

function onSavePrg() {
  // If this is for a bed and if the bed has assigned channels. Assign those chnls here.
  document.getElementById("cureAction").value = 'save';// Tell the controller to save the program to the Database.
  if(prgWhat == "Bed") {              // Is a bed being programmed?
    assignBedChnls();                 // Assign any bed chnls that need assigning.
    if(startBedChnlDex != 0) {        // Have all the bed chnl that need assigning been assigned.
      return;                         // Returning here causes the next bed chnl to be assigned.
    } // End of assigning bed chnls.
  } // End of if a bed is being programmed.
  document.getElementById("submitBtn").click();// Cause the controller to save the program.
}

// This function converts a five character string "hh:mm" into total elapsed minutes or a string
// without a ":" and only numbers into elapsed minutes.
function timeTxtToMinutes(timeTxt) {
  if(timeTxt.search(':') >= 0) {        // Is there a ":" in the given string?
    var hourTxt = timeTxt.substr(0, 2); // Yes. The first two characters are always the elapsed time's hours.
    var minTxt = timeTxt.substr(3,2);   // The fourth and fifth characters are always the elapsed time's minutes.
    var hours = parseInt(hourTxt);      // Convert the hours characters into an hours integer.
    var elapsedMinutes = hours * 60;    // Convert the hour integer into the number of minutes elapsed.
    var minutes = parseInt(minTxt);     // Convert the minutes characters into a mintues integer.
    elapsedMinutes += minutes;          // Combines the elapsed minutes for the hours and minuttes to get the total elapsed minutes.
    return elapsedMinutes;              // Provide the caller with the number of elapsed minutes.
  } else {                              // The time entered by the user does not contain a ':'.
    if(isNaN(timeTxt) == true) {        // Is the entered time a number?
      return -60;                       // No. Returning -60 indicates an invalid time was entered.
    }                                   // The eneterd time was a number. It should be interpretted as elapsed minutes.
    return parseInt(timeTxt);           // Convert the number string into a number.
  } // End of converting a string without a ":".
} // End of function timeTxtToMinutes().

// Respond to the user hitting the Tab key or the Enter key in the program point table.
// We want these keys to regenerate the graph.
function tableKeyDown(event) {
  var keyPressed = event.key;           // Get the key pressed.
  if(keyPressed == "Tab" || keyPressed == "Enter") { // Was it Tab or Enter?
    document.getElementById("submitBtn").click();// Yes. Redraw the graph.
  }                                     // It was some other key. Ignore it.
}

function pointInsert() {              // The user wants to insert a point in front of the selected point.
  // The inserted point will be half way between the given point and the previous point.
  var minSpan = prgChart[theEditDex].x - prgChart[theEditDex - 1].x;// Calculate the hours since the previous point.
  minSpan *= 60;                      // Convert the hours into minutes.
  minSpan /= 2;                       // Calculate half the span in minutes.
  minSpan = Math.round(minSpan);      // Get rid of any fraction.
  var newHours = minSpan / 60;        // Calculate hlf the span in hours.
  var tmpSpan = prgChart[theEditDex].y;// Get this point's temperature.
  var tmpBase = prgChart[theEditDex - 1].y;// Get the last point's temperature.
  tmpSpan -= tmpBase;                 // Calculate the change in temperature.
  tmpSpan = Math.round(tmpSpan / 2);  // Calculate half the change in temperature.
  var newTemp = parseInt(tmpBase) + parseInt(Math.round(tmpSpan));// Calculate the temperature of the point to insert.
  var newPoint = {x:newHours, y:newTemp};// Create the new point to insert.
  prgChart.splice(theEditDex, 0, newPoint);// Insert the new point.
  document.getElementById("submitBtn").click();// Actuate the submit btn to casue the new program point to be displayed.
} // End of function pointInsert();

function pointDelete() {
  prgChart.splice(theEditDex, 1);               // Remove the point at the given index.
  document.getElementById("submitBtn").click(); // Update the chart.
}

function pointAdd() {          // The user wants to add a new point to the end of the program.
  // Create a new last point. It cannot be copied. The values from the current last
  // point must be used to create a new last point.
  var lastPoint = {x:prgChart[prgChart.length - 1].x + 2, y:prgChart[prgChart.length - 1].y};
  prgChart.push(lastPoint);   // Append the new last point.
  document.getElementById("submitBtn").click();// Update the chart.
}

document.getElementById("closeEdit").onclick = function() {// The user is cancelling the edit program dialog.
  document.getElementById("editPrg").style.display = "none";// Hide the edit program dialog.
}

document.getElementById("closeAssignChnl").onclick = function() {// The user is cancelling the assign chnl dialog.
  document.getElementById("assignChnl").style.display = "none";// Hide the assign chnl dialog.
}

document.getElementById("insertPntBtn").onclick = function() {// The user is insert a point between two points.
  pointInsert();                      // Insert the point.
}

document.getElementById("deletePntBtn").onclick = function() {// The user is deleting a point.
  pointDelete();                      // Delete the point.
}

document.getElementById("addPntBtn").onclick = function() {// The user is adding a point to the end of the points.
  pointAdd();                         // Add the point.
}

document.getElementById("cancelPntBtn").onclick = function() {// The user is cancelling the edit program dialog.
  document.getElementById("editPrg").style.display = "none";// Hide the edit program dialog.
}

document.getElementById("warningStorageOKBtn").onclick = function() {// The user is OK with the action in spite of the warning.
  peLS['isUsed'] = 'true';            // This user is going to use local storage.
  loadFromLocalStorage();             // Create all the local storage space with defaults.
  submitForm();                       // Submit the changes to the server.
} // End of function to allow this user to use local storage.

document.getElementById("warningStorageCancelBtn").onclick = function() {// The user doesn't want local storage.
  peLS['isUsed'] = 'false';           // Local storage is not to be used.
  submitForm();                       // Submit the desired storage.
} // End of function to decline to use of local storage.

function submitForm() {               // Submit the form to the server.
  document.getElementById("cureAction").value == 'Update';
  // Convert the program object into a JSON string.
  document.getElementById("thePrg").value = JSON.stringify(prgData);
  // Convert the bed configuration object into a JSON string.
  document.getElementById("theBeds").value = JSON.stringify(bedConfigs);
  // Convert the program points object to a JSON string.
  document.getElementById("thePrgPoints").value = JSON.stringify(prgChart);// Set the hidden input for the program points (Laravel thing).
  // Convert the session configuration object into a JSON string.
  document.getElementById("peLS").value = JSON.stringify(peLS);
  document.getElementById("cureForm").submit();// Submit the form to the server (PagesController).
}

window.onload = onLoadSetBtns();      // Initialize the buttons.
window.onload = createChnlList("chnlCtrlHeader", "chnlSelect");// Initialize the chnl list.
window.onload = createChnlList("refHeader", "refSelect");// Initialize the reference chnl list.
window.onload = createPointTable();   // Create the point table.
window.onload = drawChart();

function onLoadSetBtns() {            // Initialize the buttons.
  console.log("Setting buttons.");
  if(peLS['isUsed'] == 'unknown' || peLS['isUsed'] == null) {// Has the use of local storage been determined?
    checkLocalStorage();
    if(peLS['isUsed'] == 'unknown') { // Condition: Local storage been evaluated. Is local storage unknow?
      // Yes. Use the warningDlg to ask the user if they would like to use local storage.
      var warningDlg = document.getElementById("warningStorageModal");
      warningDlg.style.display = "block"; // Display the warning dialog.
    } // End of condition: Unknown usage of local storage.
  }

  if(prgWhat == "Bed") {              // Is a bed being programmed?
    document.getElementById("bedChnlBtn").value = "Bed:";// Yes. Put Bed on the button.
  } else {                            // A chnl is being programmed.
    document.getElementById("bedChnlBtn").value = "Channel:";// Put Channel on the button.
  }                                   // The button that indicates what is being programmed has been labeled.
  if(prgData[0].active == 0) {        // Is the program active?
    document.getElementById("stopBtn").disabled = true;// No. Disable the button to stop the program.
    document.getElementById("stopBtn").value = "off";// Set it to display off.
    document.getElementById("stopBtn").style.color = "lightgray";// Lightgray is for disabled.
    document.getElementById("stopBtn").style.borderColor = "lightgray";// Lightgray is for disabled.
  } // End of disabling the button to stop a program which isn't active.
  var theST = prgData[0].timeToStart.replace("T", " ");
  // Create a string containg the date in US format.
  var usDate = theST.substr(5, 2) + "-";// The US date date starts with the month.
  usDate += theST.substr(8, 2) + "-";// It then contains the day.
  usDate += theST.substr(0, 4);// Finally, it contains the year.
  usDate += " " + theST.substr(11, 5);
  document.getElementById("start_time").value = usDate;
  jQuery('#start_time').datetimepicker({ // Configure the datetimepicker.
    format:'m-d-Y H:i',
    //format:'m-d-Y',                     // Set the display format to a US date with no time.
    //  inline:true,
    timepicker:true,                   // Show the timepicker.
    inline:false,                       // It is not to come up in drop down format.
    //startDate:theST,             // Set the date using ISO 8601 YYYY-MM-DD.
    onChangeDateTime:changeStartTime,   // Call an event handler if the user changes the date.
  }); // The datetimepicker is configured,

} // End of function onLoadSetBtns().

function drawChart() {                  // Draw the time vs temperature chart.
  // Chart.js requires the HTML canvas element in order to build the chart.
  var ctx = document.getElementById("myPrgChart").getContext('2d');
  var ttSpan = 24;                        // The default time span in hours.
  var chartMaxTemp = 180;                 // The default maximum temperature label.
  var chartMinTemp = 0;                   // The default minimum temperature label.
  var chartTempAxisLabel = "Temperature"; // The default temperature axis label.
  if(prgData[0].type == 2) {              // Is this program a child?
    chartMaxTemp = 80;                    // Yes. Set a differential graph
    chartMinTemp = -80;                   // between -80 and 80.
    chartTempAxisLabel = "Differential Temperature";// Set a differential label.
  } // End of condition: This a child program.

  var xLabel = "Elapsed hours"; // The label for the X axis.
  var prgmObjs = [];            // An array of items to plot. Includes all display settings and the time/temperature points.
                                // "prgChart" contains x and y coordinates (time and temperature coordivates) for
                                // each program that is to be plotted.
  var colorStr = peLS['PE_Prgm_Color'][prgDex];// Get this program's color.

  prgmPlotSettings = {              // Create the plot settings for this program.
    label: chartLabel,              // This is the label to put at the chart's top.
    fill: false,                    // Don't fill under the line.
    lineTension: 0.0,               // 0.0 is for straight lines. As the number goes up the line becomes curvy.
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
    data: prgChart,                 // The program data created by the server (Page Controller).
  } // End of the settings for this program.
  prgmObjs.push(prgmPlotSettings);  // The array of program point objects to plot.

  var myChart = new Chart(ctx, {      // Create the chart.
      type: 'line',                   // The chart is a line chart.
      data: {                         // The data to plot.
        datasets:                     // The datasets to plot.
          prgmObjs                    // The objects previously created.
      },                              // The end of the data to plot.
      options: {                      // The options for creating the chart.
          animation: {                // Disable animation. Animation is a problem
            duration: 0               // if the plot is for a long time period.
          },
          maintainAspectRatio: true,  // boolean true Maintain the original canvas aspect
                                      // ratio (width / height) when resizing.
          scales: {                   // see https://www.chartjs.org/docs/latest/charts/line.html?q=scales
            xAxes: [{
  						type: "linear",         // Use elapsed minutes (a number that starts at zero and goes up).
                                      // configure how different units are formed into strings for the
                                      // axis tick marks.
  						display: true,          // Display vertical grid lines.
  						scaleLabel: {
  							display: true,        // Display the x axis label.
  							labelString: xLabel,  // Set the x axis label.
  						},
              ticks: {
                min: 0,               // The left side of the axis always starts at zero elapsed hours.
                max: ttSpan,          // The width (number of hours) of the 'x' axis.
                stepSize: 2           // Display a vertial grid line every two hours.
              }
  					}],
            yAxes: [{
              scaleLabel: {
  							display: true,        // Display the Y axis label.
  							labelString: chartTempAxisLabel,// Set the Y axis label.
  						},
              ticks: {
                  max: chartMaxTemp,  // The top temperture coordinate.
                  min: chartMinTemp,  // The bottom temperture coordinate.
              }
            }]
          }
      }

  });

}

</script>

<!--<script src="js/cureChart.js"></script>-->

@endsection
