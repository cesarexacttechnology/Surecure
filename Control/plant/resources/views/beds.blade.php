@extends('master')
@section('title', 'Home')

@section('content')

<head>

  <!-- http-equiv="refresh" content="1800;url=/beds" causes the page to be
      refreshed every 1800 seconds (30 minutes) and to use a GET instead of a POST. -->
  <meta charset="UTF-8" http-equiv="refresh" content="1800;url=/beds">

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

  .bedBtns {
    border-width: 1px;
    border-radius: 4px;
    border-color: black;
    font-family: Arial, Helvetica, Sans-serif;
    font-size: 16px;
    margin-top: 10px;
    margin-left: 5px;
    min-width: 100px;
  }

  .bedBtnLoc {
    display: flex;
    justify-content: center;
  }

  .bedBtnHide {
    display: none;
  }

  .bedBtnShow {
    display: flex;
    justify-content: center;
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

  li{
    list-style-type: none;
  }

  label { display: block; color: WindowText; background-color: Window; margin: 0; padding: 0px 0px 0px 2px; width: 100%; }
  label:hover { background-color: Highlight; color: HighlightText; }

  #bName {
    margin: 0px 5px;
    border-style: solid;
    border-width: thin;
    border-color: gray;
    border-radius: 4px;
    padding: 1px 3px;
    background-color: rgba(0,0,0,0.001);
    width: 110px;
    text-align: center;
  }

  #addChnlColumnHeadings, #editColumnHeadings {
    margin-top: 20px;
  }

  #selectBedChnl {
    margin-left: 25px;
  }

  </style>

</head>

<script src="/js/PELocalStorage.js"></script>

<body>

<form id="bedForm" method="post">

  <!-- Display any errors. -->
  @foreach ($scErrors as $error)
    <p class="alert alert-danger">{{ $error }}</p>
  @endforeach


<!-- The next line submits the token required by Laravel -->
<input type="hidden" name="_token" value="{!! csrf_token() !!}">
<!-- Create a hidden input to hold the action to be performed during a bed update. -->
<input type="hidden" id="theAction" name="theAction" value='none'>
<!-- Create a hidden input to hold the bed configurations. -->
<input type="hidden" id="theBeds" name="theBeds">
<!-- Create a hidden input to hold the session configuration. -->
<input type="hidden" id="peLS" name="peLS">

@if (session('status'))
  {{ session('status') }}
@endif

<h1 class="bedBtnLoc">Bed Configuration</h1>

<div id="bedDiv"></div>
<div class="bedBtnLoc">
    <button class="bedBtnShow bedBtns" id="aBedBtn" type="button" onclick="addBed()">Add Bed</button>
    <button class="bedBtnHide bedBtns" id="rBedBtn" type="button">Remove Bed</button>
    <button class="bedBtnHide bedBtns" id="aBedChnlBtn" type="button">Add Channel</button>
    <button class="bedBtnHide bedBtns" id="eBedChnlBtn" type="button">Edit Channel</button>
    <button class="bedBtnHide bedBtns" id="rBedChnlBtn" type="button">Remove Channel</button>
    <button class="bedBtnHide bedBtns" id="saveBedsBtn" type="submit">Save</button>
    <button class="bedBtnHide bedBtns" id="cancelBedBtn"  type="button" onclick="location.href='{{ url('/beds')}}'">Cancel</button>
    <button class="bedBtnShow bedBtns" id="homeBtn" type="button" onclick="location.href='{{ url('/') }}'">Home</button>
</div>
<p id="instruct" class="bedBtnShow bedBtns">Select a bed or bed channel to edit.</p>
</form>

<!-- The modal dialog to add a chnl to a bed. -->
<div id="addChnlModal" class="PEmodal">
  <!-- modal content -->
  <div class="PEmodal-content">
    <span id="closeBedChnl" class="PEclose">&times;</span>
    <h4>Select a channel and configuration to add to
    <span id="AddChnlBedName">waiting</span>
    <div id="addChnlColumnHeadings" style="margin-left:38px">Channel
      <span style="margin-left:40px">Type</span>
      <span id="labelRefSelect" style="margin-left:38px">Reference</span>
    </div>
    <div id="selectBedChnl"></div>
    </h4>
    <button id="addBedChnlBtn" class="bedBtns" style="margin-left:38px">Add</button>
    <button id="cancelAddBedChnlBtn" class="bedBtns">Cancel</button>
  </div>
</div><!-- End of the dialog to add a chnl to a bed. -->

<!-- The modal dialog to edit a bed chnl. -->
<div id="editChnlModal" class="PEmodal">
  <!-- modal content -->
  <div class="PEmodal-content">
    <span id="closeEditBedChnl" class="PEclose">&times;</span>
    <h4>Edit channel
    <span id="editChnlName">waiting</span>
    <div id="editColumnHeadings" style="margin-left:38px">Type
      <span id="editRefLabel" style="margin-left:38px">Reference</span>
    </div>
    <div id="editBedChnl"></div>
    </h4>
    <button id="editBedChnlBtn" class="bedBtns" style="margin-left:38px">OK</button>
  </div>
</div><!-- End of the dialog to edit a bed chnl. -->

<!-- The modal dialog to get a bed's name. -->
<div id="bedNameModal" class="PEmodal">
  <!-- modal content -->
  <div class="PEmodal-content">
    <h3>Add a bed</h3>
    <h4>What should this bed be called?</h4>
    <input id="bedName" name="bedName" type="text">
    <button id="bedNameOKBtn" class="bedBtns">OK</button>
    <button id="cancelBedNameBtn" class="bedBtns">Cancel</button>
  </div>
</div><!-- End of the dialog to add a chnl to a bed. -->

<!-- The modal dialog to get a yes or no response. -->
<div id="yesNoModal" class="PEmodal">
  <!-- modal content -->
  <div class="PEmodal-content">
    <h4 id="yesNoMessage">Message</h4>
    <button id="yesBtn" class="bedBtns">Yes</button>
    <button id="noBtn" class="bedBtns">No</button>
  </div>
</div><!-- End of the dialog to get a yes or no response. -->

<!-- The modal dialog to send a warning to the user. -->
<div id="warningModal" class="PEmodal">
  <!-- modal content -->
  <div class="PEmodal-content">
    <h4 id="warningMessage">Message</h4>
    <button id="warningBtn" class="bedBtns">OK</button>
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

<script>
///////////////////Global/Variables//////////////////////////////////////////
  var bedConfigs = <?php echo json_encode($theBeds); ?>;// Put the bed configurations into a json string.
  var chnls = <?php echo json_encode($chnls); ?>;// Put the chnl configurations into a json string.
  var peLS = <?php echo json_encode($peLS); ?>;// Contains this session's configuration.
                                      // The default is not set until local storage is checked.
  var bColor = "White";               // Contains the backgroundColor of the dropdown behind the mouse cursor.
  var theBedDex = -1;                 // The selected bed index, default (-1) is invalid.
  var theChnlDex = -1;                // The selected bed chnl index, default (-1) is invalid.
  var isChnlSelected = false;         // Indicates a bed chnl has been selected.
  var isBedChanged = false;           // Indicates that some change has been made to at least one of the beds.
  var yes_action;                     // The function to call if the yes button is clicked.
  var isAddBedChnl = false;           // By default, a bed chnl is not being added.
///////////////End/Global/Variables//////////////////////////////////////////

  function addBed() {                 // The user wants to create a new bed.
    getNewBedName();
  }

  function addBedWithName() {
    // First, create a new default bed chnl object for the bed. Any bed must have at least one chnl.
    var newBedChnl = {scChnlDex:"0", bedChnlRefDex:"-1", controlType:"1", assignedChnlDex:"-1" };
    var noBeds = bedConfigs.noBeds + 1;// Increment the total number of beds by one.
    // Next create a new bed object. The bed will only have one chnl.
    var newBedName = document.getElementById("bedName").value;
    console.log("The new name is " + newBedName);
    //var newBed = {name: noBeds.toString(), noChnls:1, bedChnl0:newBedChnl};
    var newBed = {name: newBedName, noChnls:1, bedChnl0:newBedChnl};
    // Create the key name for the new bed object. It must be compatible with the other bed key names. (bed?)
    theBedDex = bedConfigs.noBeds;    // The index of the bed to be added is the number of beds that currently exist.
    theChnlDex = 0;                   // The index of the new bed's chnl is the first index.
    console.log("theBedDex is " + theBedDex.toString());
    var bedObjName = "bed" + (bedConfigs.noBeds).toString();// Create the new bed's key.
    bedConfigs[bedObjName] = newBed;  // Append the new bed to the bedConfigs objects.
    bedConfigs.noBeds = noBeds;       // Correct the number of beds.
    isAddBedChnl = false;             // A default bed chnl has been created. It is just going to be modified.
    addChnl(theChnlDex);              // Add a default chnl (the first chnl) to this new bed.
    isBedChanged = true;              // The beds have been changed.
    pageReset();                      // Reset the beds page.
  }

  function redrawTable() {
    var bedDiv = document.getElementById("bedDiv");// Get the parent for the bed table.
    var bT = document.getElementById("theBedTable");// Get the bed table.
    bedDiv.removeChild(bT);           // Remove the bed table from its parent.
    var saveBtn = document.getElementById("saveBedsBtn");// The save button needs to be enabled.
    saveBtn.className = "bedBtnShow bedBtns";// A bed has been added. The user needs to be able to save the bed.
    drawTable();                      // Recreate the bed table which now includes the new bed.
  }

  function getNewBedName() {
    document.getElementById("bedNameModal").style.display = "block";// Display the modal dialog to select a bed chnl.
    document.getElementById("bedName").focus();// Set the focus to the text box used to enter the new bed's name.
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

  var yesReplyBtn = document.getElementById("yesBtn");// Access the yes no dialog yes button.
  yesReplyBtn.onclick = function()  { // The user clicked on the yes button.
    var yesNoDlg = document.getElementById("yesNoModal");// Get acces to the yes no dialog.
    yesNoDlg.style.display = "none";  // Remove the yes no dialog.
    yes_action();
  } // End of function getYesReply().

  var noReplyBtn = document.getElementById("noBtn");// Access the yes no dialog no button.
  noReplyBtn.onclick = function () {  // The user clicked on the no button.
    var yesNoDlg = document.getElementById("yesNoModal");// Get acces to the yes no dialog.
    yesNoDlg.style.display = "none";  // Remove the yes no dialog.
    pageReset();                      // Go back to the initial beds page.
  }

  var warningReplyBtn = document.getElementById("warningBtn");// Access the warning dialog button.
  warningReplyBtn.onclick = function() {// The user acknowledged the warning.
    var warningDlg = document.getElementById("warningModal");// Get acces to the warning dialog.
    warningDlg.style.display = "none";// Remove the warning dialog.
    pageReset();                      // Go back to the initial beds page.
  }

  function drawTable() {              // Displays the beds with all thier chnls in tabular format.
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
    var bedDiv = document.getElementById("bedDiv");// Get the location for the table.
    var bedTbl = document.createElement("table");// Create the bed table.
    bedTbl.border = 1;                // Give the table a visible border.
    bedTbl.style.marginLeft = "auto"; // Center the table
    bedTbl.style.marginRight = "auto";// in the horizontal direction.
    bedTbl.setAttribute("id", "theBedTable");// Allow access to the table through its id.
    var hRow = bedTbl.insertRow(-1);  // Create a header row in the table.
    var hTxt = ["Bed", "Channel", "Control Type", "Ref Channel"];// Labels for the header row.
    for(var c = 0; c < 4; c++) {      // Create a cell for each of the four columns.
      var hCell = document.createElement("th");// Create the header cell.
      hCell.style.padding = "6px";    // Create space around the label for readability.
      if(c == 0 ) {                    // Is the the bed name column?
        hCell.style.paddingLeft = "30px";// Yes. Add extra space to the left
        hCell.style.paddingRight = "30px";// and to the right to allow for larger bed names.
      } else if(c == 1) {             //  Is the the bed chnl name column?
        hCell.style.paddingLeft = "20px";// Yes. Add extra space to the left
        hCell.style.paddingRight = "20px";// and to the right to allow for larger bed chnl names.
      }
      hCell.innerHTML = hTxt[c];      // Set this column's header.
      hRow.appendChild(hCell);        // Add this column's header cell to the header row.
    }                                 // The column labels have been added to the bed table.
    console.log("Drawing the bed table with " + bedConfigs.noBeds + " beds.");
    console.log(bedConfigs);
    for(var r = 0; r < bedConfigs.noBeds; r++) {// Go through each bed.
      var bTxt = "bed" + r;           // Used to access this bed from the bed configurations passed by the controller.
      var noChnls = bedConfigs[bTxt].noChnls;// Get this bed's number of chnls.
      for(var cDex = 0; cDex < noChnls; cDex++){// Go through this bed's chnls.
        var row = document.createElement("tr");// Create a row in the bed table to show this bed chnl's configuration.
        var bcTxt = "bedChnl" + cDex; // Used to access this bed chnl's configuration passed by the controller.
        var bedChnl = bedConfigs[bTxt][bcTxt];// Used to access this bed chnl's configuration.
        var scChnlDex = Number(bedChnl.scChnlDex);// Contains this bed chnl's scChnnl index.
        for(var c = 0; c < 4; c++) {  // Go through the four settings for this bed chnl.
          var cell = document.createElement("td");// Create a cell to hold part of this bed chnl's configuration.
          if (r % 2 == 0) {           // Is this a bed with an even bed index?
            cell.style.backgroundColor = "gainsboro";// Yes. Change the backgroundColor so the table is easier to read.
          } else {                    // This is a bed with an odd bed index.
            cell.style.backgroundColor = "white";// Change the backgroundColor to be different than the beds with an even index.
          }                           // The backgroundColor has been set to make the table more readable.
          if(c == 0) {                // Is this the column for the bed's name?
            var cellText = document.createTextNode(bedConfigs[bTxt].name);// Create a text node to hold this bed's name.
            cell.style.textAlign = "center";// Center the name on the table's cell.
            cell.appendChild(cellText);// Add the bed's name to the tabel's cell.
          } else if (c == 1) {        // Is this the column for the bed chnl's name?
            var cellText = document.createTextNode(bedChnlName(r, cDex));// Put its name in a text node.
            if (scChnlDex >= 0) {     // Is this a regular chnl?
              var scChnl = chnls[scChnlDex];// Get this chnls scChnl configuration.
              var colorTxt = peLS['PE_Chnl_Color'][scChnlDex];// Get this chnl's color.
            }                         // The chnl's color has been set.
            cell.style.textAlign = "center";// Center the chnl's name in the cell.
            cell.appendChild(cellText);// Make the chnl's name a part of this table cell.
            cell.style.color = colorTxt;// Set the color for the chnl.
          } else if (c == 2) {        // Is this the column for the bed chnl's control type?
            var ctrlTxt = ["----", "parent", "child", "cutoff", "mimic", "cylinder", "none"];// The types.
            var text = document.createTextNode(ctrlTxt[bedChnl.controlType]);// Create its text.
            cell.style.textAlign = "center";// Center the control type name in the table cell.
            cell.appendChild(text);   // Add the control type's name to the tabel's cell.
          } else {                    // Create a cell to contain the reference chnl linked to this bed chnl.
            cell.style.textAlign = "center";// Center the reference name on the table's cell.
            var cellText = document.createTextNode("-----");// The default name is nothing (parents don't have references).
            colorTxt = "#000000";     // The default color for the reference is black.
            if (bedChnl.controlType != 1) {// Is the control type something other than a parent?
              // Yes. This bedChnl's bedChnlRefDex contains the bed chnl index this bedChnl is referenced to.
              // Create the key to this bedChnl's reference.
              cellText.nodeValue = bedChnlName(r, bedChnl.bedChnlRefDex);// Get the bed chnl name for the reference.
              var refKey = "bedChnl" + bedChnl.bedChnlRefDex.toString();// Create the key for the reference.
              var refSCChnlDex = Number(bedConfigs[bTxt][refKey].scChnlDex);// Get the reference's scChnlDex.
              if(refSCChnlDex != -1) {// -1 implies it is assigned at progrma time.
                var rChnl = chnls[refSCChnlDex];// Access the scChnl which is the reference.
                colorTxt = peLS['PE_Chnl_Color'][refSCChnlDex];// Get this chnl's color.
              }                       // The reference's color has been set.
            }                         //
            cell.style.color = colorTxt;// Set the cell's color to the reference chnl's color.
            cell.appendChild(cellText);// Add the reference chnl's name to the tabel's cell.
          }                           // Each chnl's four configuration items is in a cell.
          cell.setAttribute("id", r + "-" + cDex + "-" + c);// Set the id attribute so this cell can be accessed.
          cell.onmouseup = function(){modifyBed(event)};// Allow the user to modify this bed.
          // Cause this cell's background color to change when the mouse is over it.
          cell.onmouseover = function(){enterCell(event)};
          cell.onmouseleave = function(){cellLeave(event)};// Remove the cell's highlighting when the mouse is not over it.
          row.appendChild(cell);      // Add this cell to this table row.
        }                             // The four settings for this bed chnl have been placed in this chnl's row.
        bedTbl.appendChild(row);      // Add this chnl's row to the table.
      }                               // All this bed's chnls have table rows.
    }                                 // All the bed configurations have put in rows.
    bedDiv.appendChild(bedTbl);       // Put the table on the page.
    isChnlSelected = false;           // The table was just drawn. Nothing is selecetd.
  } // End of function drawTable().

  function modifyBed(event) {         // The user is modifying a bed.
    console.log("The id is " + event.target.id);
    var dash = event.target.id.indexOf("-");//The bedDex and chnlDex are sperated by a "-" in the id.
    // We need to know the location of the dashes in the id to extract the bedDex and the chnlDex.
    var dash2 = event.target.id.indexOf("-", dash + 1);// Get the location of the second dash.
    theBedDex = Number(event.target.id.substr(0, dash));// Extract the bedDex.
    console.log("theBedDex is " + theBedDex);
    theChnlDex = Number(event.target.id.substr(dash + 1, dash2 - dash - 1));// Extract the chnlDex.
    console.log("The chnlDex is " + theChnlDex);
    var addBtn = document.getElementById("aBedBtn");// The user is not adding a bed.
    addBtn.className = "bedBtnHide";  // Hide the button to add a bed.
    var remBedBtn = document.getElementById("rBedBtn");// Access the "Remove Bed" button.
    remBedBtn.onmouseup = function(){removeBed()};// Set the function that removes a bed.
    remBedBtn.className = "bedBtnShow bedBtns";// Display the "Remove Bed" button.
    var addChnlBtn = document.getElementById("aBedChnlBtn");// Access the "Add Channel" button.
    addChnlBtn.onmouseup = function(){addChnl(-1)};// Set the function that adds a chnl. (-1 => let the user choose, no default)
    addChnlBtn.className = "bedBtnShow bedBtns";// Display the "Add Channel" button.
    var editChnlBtn = document.getElementById("eBedChnlBtn");// Access the "Edit Channel" button.
    editChnlBtn.onmouseup = function(){editChnl()};// Set the function that edits a chnl.
    editChnlBtn.className = "bedBtnShow bedBtns";// Display the "Edit Channel" button.
    var remChnlBtn = document.getElementById("rBedChnlBtn");// Access the remove bed chnl button.
    remChnlBtn.onmouseup = function(){removeChnl()};// Set the function that removes a chnl.
    remChnlBtn.className = "bedBtnShow bedBtns";// Display the "Remove Channel" button.
    var bTxt = "bed" + theBedDex;     // Set the key to access the selected bed.
    if(bedConfigs[bTxt].noChnls == 1) {// Does this bed only have one chnl?
      remChnlBtn.className = "bedBtnHide";// Yes. Don't allow the user to remove its only chnl.
    }                                 // The Remove Channel button is displayed based on the number of bed chnls.
    var cancelBtn = document.getElementById("cancelBedBtn");// Access the cancel bed modification button.
    cancelBtn.className = "bedBtnShow bedBtns";// Display the cancel bed modification button.
    document.getElementById("instruct").innerHTML = "Edit " + bedConfigs[bTxt].name + ".";// Display the bed being modified.
    isChnlSelected = true;            // modifyBed() is only called if a chnl is selected.
    // The following loop assures the backgroundColor of each cell is set correctly.
    for(var bDex = 0; bDex < bedConfigs.noBeds; bDex++) {// Go through each bed.
      bTxt = "bed" + bDex;            // Create the key name for the bed.
      var theBed = bedConfigs[bTxt];  // Get access to the bed.
      for(var cDex = 0;  cDex < theBed.noChnls; cDex++) {// Go through each of the bed's chnls.
        for(var col = 0; col < 4; col++) {// Go through each of the four columns.
          var id = bDex.toString() + "-" + cDex.toString() + "-" + col.toString();// Create the cell's id.
          var cell = document.getElementById(id);// Access the cell.
          if(bDex == theBedDex && cDex == theChnlDex) {// Is this one of the selected cells?
            cell.style.backgroundColor = "F3F781";// Set the backgroundColor to a light yellow.
          } else {                    // This is not a selected cell. Its backgroundColor is a function of even or odd bed.
            if (bDex % 2 == 0) {      // Is this a bed with an even bed index?
              cell.style.backgroundColor = "gainsboro";// Yes. Change the backgroundColor so the table is easier to read.
            } else {                  // This is a bed with an odd bed index.
              cell.style.backgroundColor = "white";// Change the backgroundColor to be different than the beds with an even index.
            }                         // The backgroundColor has been set to make the table more readable.
          } // End of backgroundColor for a normal cell.
        } // End of going through each column.
      } // End of going through each bed chnl.
    } // End of going through each bed.
  } // End of function modifyBed().

  function removeBed() {              // The user wants to remove the selected bed.
    var bTxt = "bed" + theBedDex;     // Create the bedConfigs key for this bed.
    var theBed = bedConfigs[bTxt];
    var yesNoDlg = document.getElementById("yesNoModal");
    var message = "Remove bed " + theBed.name + "?";
    document.getElementById("yesNoMessage").innerHTML = message;
    yes_action = removeBed2;
    yesNoDlg.style.display = "block";// Display the modal dialog to select a bed chnl.
  }

  function removeBed2() {
    bedConfigs.noBeds--;              // Correct the number of beds.
    for (var bDex = theBedDex; bDex < bedConfigs.noBeds; bDex++) {
      var newBedTxt = "bed" + bDex;
      var bNum = Number(bDex + 1);
      var oldBedTxt = "bed" + bNum;
      bedConfigs[newBedTxt] = bedConfigs[oldBedTxt];
    }
    oldBedTxt = "bed" + bedConfigs.noBeds;
    delete bedConfigs[oldBedTxt];
    isBedChanged = true;              // The beds have been changed by removing a bed.
    pageReset();
  }

  function pageReset() {
    var bedDiv = document.getElementById("bedDiv");// Get the parent for the bed table.
    var bT = document.getElementById("theBedTable");// Get the bed table.
    bedDiv.removeChild(bT);           // Remove the bed table from its parent.
    var saveBtn = document.getElementById("saveBedsBtn");// The save button needs to be shown.
    if(isBedChanged == true) {        // Have there been changes to the beds?
      saveBtn.className = "bedBtnShow bedBtns";// Yes. Show the Save button.
    } else {                          // No. There have been no changes to the beds.
      saveBtn.className = "bedBtnHide";// Hide the Save button.
    }                                 // The display state of the Save button is set.
    var addBtn = document.getElementById("aBedBtn");// The user is allowed to add a bed after removing a bed.
    addBtn.className = "bedBtnShow bedBtns";// Show the button to add a bed.
    var remBedBtn = document.getElementById("rBedBtn");// The remove bed button no longer makes sense.
    remBedBtn.className = "bedBtnHide";// Hide the remove bed button.
    var addChnlBtn = document.getElementById("aBedChnlBtn");// The add bed chnl button no longer makes sense.
    addChnlBtn.className = "bedBtnHide";// Hide the add bed chnl button.
    var editChnlBtn = document.getElementById("eBedChnlBtn");// The edit bed chnl button no longer makes sense.
    editChnlBtn.className = "bedBtnHide";// Hide the add bed chnl button.
    var remChnlBtn = document.getElementById("rBedChnlBtn");// The remove bed chnl button no longer makes sense.
    remChnlBtn.className = "bedBtnHide";// Hide the remove bed chnl button.
    var cancelBtn = document.getElementById("cancelBedBtn");// The cancel button no longer makes sense.
    cancelBtn.className = "bedBtnHide";// Hide the cancel button.
    drawTable();                      // Recreate the bed table which no longer includes the removed bed.
  }

  function editChnl() {
    var editDiv = document.getElementById("editBedChnl");// Get the div which holds the items to edit.
    while (editDiv.hasChildNodes()) { // Remove any chnl which may have previously populated
      editDiv.removeChild(editDiv.firstChild);// this div.
    }                                 // The div is empty.
    var bTxt = "bed" + theBedDex;     // Create the object key for this bed.
    bcTxt = "bedChnl" + theChnlDex;   // Create the key to access this bed chnl's configuration.
    // Show the user the name of the channel they are editing in case they forgot.
    document.getElementById("editChnlName").innerHTML = bedChnlName(theBedDex, theChnlDex) + ".";
    // bookmark
    var selType = document.createElement("select");// Create a selector containing all control types.
    selType.setAttribute("size", 4);  // The selector displays 4 types even though there may be more.
    var ctrlTxt = ["----", "parent", "child", "cutoff", "mimic", "cylinder"];// The choices.
    // Create each possible choice in the selector.
    for (var i = 1; i < ctrlTxt.length; i++) {// Go through each possible choice.
      var opt = document.createElement("option");// Create the choice.
      var text = document.createTextNode(ctrlTxt[i]);// Create its text.
      if (i == bedConfigs[bTxt][bcTxt].controlType) {// Is this the chnl's control type?
        opt.selected = true;          // Yes. Set its "selected" property.
      }                               // The selected controlType has been evaluated.
      opt.appendChild(text);          // Add the text to the choice.
      selType.appendChild(opt);       // Add the choice to the selector.
    }                                 // The type selector has been created.
    selType.style.width = "78px";     // Set a uniform width for the selector.
    selType.style.marginLeft = "10px";// Put some space between the type selector and the dialog's border.
    selType.style.borderRadius = "4px";// Round the corners of the selector.
    selType.id = "editBedChnlType";   // Allow access to edit this bed chnl's type.
    if (bedConfigs[bTxt].noChnls <= 1) {// Are there insufficient chnls for a reference?
      selType.disabled = true;        // Yes. Disable the control type selector.
    } else {                          // No.
      selType.onmouseup = function(){editType(event)};// Create a control type event handler.
    }                                 // The control type selector has been set up.
    editDiv.appendChild(selType);     // Add the type selector to the edit bed chnl dialog.
    // Create a reference selector containing all this bed's chnls except for the chnl being edited.
    var selRef = document.createElement("select");
    selRef.setAttribute("size", 4);   // Create a selector which displays 4 selections even though there may be more.
    selRef.style.borderRadius = "4px";// Round the corners of the selection box.
    var existingRef = bedConfigs[bTxt][bcTxt].bedChnlRefDex;// Get the existing reference. There might be none (-1).
    for(var bChnlDex = 0; bChnlDex < bedConfigs[bTxt].noChnls; bChnlDex++) {// Go through each chnl on this bed.
      bcTxt = "bedChnl" + bChnlDex;     // Create the key to access this bed chnl's configuration.
      bedChnl = bedConfigs[bTxt][bcTxt];// Get this bed chnl's configuration.
      // If the chnlDex is not equal to this bedChnlDex then add this bedChnl to the list of possible reference chnls.
      if(theChnlDex != bChnlDex) {    // Is this a bed chnl which can be used as a reference?
        var opt = document.createElement("option");// Yes. Create a row in the selector for this bed chnl.
        var chnlTxt = document.createTextNode(bedChnlName(theBedDex, bChnlDex));// Put the chnl's name in the text node.
        opt.appendChild(chnlTxt);     // Make the chnl's name a part of this selector.
        if(existingRef == bChnlDex) { // Is this the currently selected reference?
          opt.selected = true;        // Yes. Highliht this selection.
        }                             // If this is the currently selected ref, it is highlighted.
        selRef.appendChild(opt);      // Make this option a part of the dropdown selection.
      }                               // The select reference list has been updated for this SCChnl.
    }                                 // The list of potential reference chnls is complete.
    selRef.style.width = "102px";     // Set a uniform width for the dropdown.
    selRef.style.marginLeft = "10px"; // Put some space between the reference and the type.
    selRef.style.borderRadius = "4px";// Round the corners of the selection box.
    selRef.id = "editBedRefChnl";     // Allow access to set this bed chnl's reference.
    if (selType.value == "parent") {  // Is the control type "parent"?
      selRef.disabled = true;         // Yes. A "parent" doesn't have a reference. Disable the refence selector.
      document.getElementById("editRefLabel").style.color = "lightgray";// Gray the reference label.
    }                                 // The reference selector has the correct enabled state.
    editDiv.appendChild(selRef);      // Make the selection list a part of the edit bed chnl dialog.
    document.getElementById("editChnlModal").style.display = "block";// Display the dialog to edit a bed chnl.
  } // End of funtion editChnl().

  function addChnl(chnlDex) {
    console.log("Going to add a channel! chnlDex = " + chnlDex );
    if (chnlDex == -1) {              // When the chnlDex is -1, a bed chnl is being added to this bed.
      isAddBedChnl = true;            // Set the flag to indicate a bed chnl is being added.
    }                                 // The flag indicating is a bed chnl is being added has been set.
    var addChnlLoc = document.getElementById("selectBedChnl");// Get the div which holds the list of chnls to add.
    while (addChnlLoc.hasChildNodes()) {// Remove any chnls which may have previously populated
      addChnlLoc.removeChild(addChnlLoc.firstChild);// this list.
    }                                 // The list contains no chnls.
    var sel = document.createElement("select");// Create a dropdown containing all this system's chnls.
    sel.setAttribute("size", 4);      // Create a selectable list which displays 3 selections even though there may be more.
    var bTxt = "bed" + theBedDex;     // Create the object key for this bed.
                                      // Display the name of the bed this chnl is a part.
    document.getElementById("AddChnlBedName").innerHTML = bedConfigs[bTxt].name + ":";
    var noChnls = bedConfigs[bTxt].noChnls;// Get the number of chnls on this bed.
    console.log("The number of chnls on this new bed is " + noChnls);
    console.log("isAddBedChnl = " + isAddBedChnl);
    var isSelected = false;           // A default chnl selection has not been set.
    // Create a list of channels which are not on this bed.
    for(var scDex = 0; scDex < chnls.length; scDex++) {// Go through each chnl on this system.
      var onBed = false;            // By default, this SCChnl is not on the bed.
      if(isAddBedChnl == true) {    // Is a bed chnl being added?
        for(var cDex = 0; cDex < noChnls; cDex++){// Yes. Go through each chnl on this bed.
          bcTxt = "bedChnl" + cDex;   // Access this bed chnl's configuration passed by the PagesController.
          bedChnl = bedConfigs[bTxt][bcTxt];// Get this bed chnl's configuration.
          if(bedChnl.scChnlDex >= 0) {// Is this bed chnl defined?
            var aChnl = chnls[bedChnl.scChnlDex];// Yes. Get the SCChnl configuration for this bed chnl.
            var bedChnlSCDex = aChnl.id - 1;// The bed Chnl's SCDex is one less than its id.
            if (bedChnlSCDex == scDex) {// Is this bed chnl the same as the SCChnl being checked?
              onBed = true;           // Yes. We don't want it on the add chnl list.
              break;                  // Since we found the chnl, there is no point in looking farther.
            }                         // This SCChnl is not the same as this bed chnl.
          }                           // It has been dtermined if this defined chnl is already on this bed.
        }                             // At this point, all the bed chnls have been compared to the SCChnl.
      }                               // Done checking to see if this chnl is already on the bed.
      if (onBed == false) {         // Is this SCChnl already on this bed?
        var opt = document.createElement("option");// No. Create a row in the dropdown for this system chnl.
        if (isSelected == false) {  // Has a default chnl been selected?
          if (isAddBedChnl == true) {// No. Is a chnl being added to this bed?
            opt.selected = true;    // Yes. Select this chnl as the default. It is the 1st chnl in the list.
            isSelected = true;      // Indicate a default chnl has been selected so a 2nd chnl won't be selected.
            chnlDex = scDex;        // Change the chnlDex so the reference list won't include this chnl.
          }                         // If a chnl is being added to a bed, the 1st chnl has been selected.
        }                          // Setting a default chnl has been processed.
        if(scDex == chnlDex && isSelected == false) {// Is the given chnlDex the same as the one being evaluated?
          opt.selected = true;        // Yes. Show this chnl as selected.
          isSelected = true;          // Remember something is selected.
        }                             // If this was the given chnl, it has been selected.
        var scChnl = chnls[scDex];    // Get this chnls scChnl configuration.
        var chnlTxt = document.createTextNode(scChnl.name);// Put its name in a text node.
        opt.appendChild(chnlTxt);     // Make the chnl's name a part of this dropdown option.
        sel.appendChild(opt);         // Make this option a part of the dropdown selection.
      }                               // The add chnl list has been updated for this SCChnl.
    }                                 // The list of potnetial chnls to add to this bed is complete.
    opt = document.createElement("option");// Create a row for an assigned chnl.
    var aTxt = noChnls.toString();    // Create the number of bed chnls as if an existing chnl is being modified.
    if(isAddBedChnl == true) {        // Is a chnl being added to the bed?
      aTxt = (noChnls + 1).toString();// Yes. increment the number of bed chnls by one.
    }                                 // The number of bed chnls is set.
    chnlTxt = document.createTextNode("Assign" + aTxt);// The assigned chnl is named Assign+bedchnlNo.
    opt.appendChild(chnlTxt);         // Add the name for an assigned chnl as the last selection option.
    sel.appendChild(opt);             // Add the assigned chnl option to the list of chnls.
    sel.style.width = "102px";        // Set a uniform width for the dropdown.
    sel.style.borderRadius = "4px";   // Round the corners of the selection box.
    sel.id = "bedChnlSelector";       // Allow access to the bed channel selector.
    addChnlLoc.appendChild(sel);      // Make the selection list a part of the modal dialog.
    var selType = document.createElement("select");// Create a dropdown containing all this control types.
    selType.setAttribute("size", 4);  // Create a selectable list which displays 4 types even though there may be more.
    var ctrlTxt = ["----", "parent", "child", "cutoff", "mimic", "cylinder"];// The choices.
    // Create each possible choice in the dropdown.
    for (var i = 1; i < ctrlTxt.length; i++) {// Go through each possible choice.
      var opt = document.createElement("option");// Create the choice.
      var text = document.createTextNode(ctrlTxt[i]);// Create its text.
      if (i == 1) {                    // Is this  a perent control type?
        opt.selected = true;          // Yes. Set its "selecetd" property.
      }                               // The selected controlType has been evaluated.
      opt.appendChild(text);          // Add the text to the choice.
      selType.appendChild(opt);       // Add the choice to the dropdown.
    }                                 // The selection dropdown has been created.
    selType.style.width = "78px";     // Set a uniform width for the dropdown.
    selType.style.marginLeft = "10px";// Put some space between the type and the chnl.
    selType.style.borderRadius = "4px";// Round the corners of the selection box.
    selType.id = "bedChnlType";       // Allow access to set this bed chnl's type.
    var chnlMinimum = 1;              // The default minimum number of chnls in order to have a reference.
    if(isAddBedChnl == true) {        // Is a chnl being added?
      chnlMinimum = 0;                // Yes. There is at least one chnl that can be referenced.
    } // The minimum number of chnl has been determined to enable a control type requiring a reference.
    if (noChnls <= chnlMinimum) {     // Are there insufficient chnls for a reference?
      selType.disabled = true;        // Yes. disable the control type selector.
    } else {                          // No.
      selType.onmouseup = function(){modifyRefSelect(event)};// Create a control type event handler.
    }                                 // The control type selector has been set up.
    addChnlLoc.appendChild(selType);  // Add the type selector to the modal dialog.
    // Create a reference selector containing all this bed's chnls except for the chnl being added.
    var selRef = document.createElement("select");
    selRef.setAttribute("size", 4);   // Create a selectable list which displays 4 selections even though there may be more.
    selRef.style.borderRadius = "4px";// Round the corners of the selection box.
    for(var bChnlDex = 0; bChnlDex < noChnls; bChnlDex++) {// Go through each chnl on this bed.
      console.log("At line 584 bChnlDex is " + bChnlDex);
      console.log("At line 584 chnlDex is " + chnlDex);
      bcTxt = "bedChnl" + bChnlDex;     // Create the key to access this bed chnl's configuration.
      bedChnl = bedConfigs[bTxt][bcTxt];// Get this bed chnl's configuration.
      // If the chnlDex is not equal to this bedChnlDex then add this bedChnl to the list of possible reference chnls.
      if(isAddBedChnl == true || chnlDex != bChnlDex) {// Is this a bed chnl which can be used as a reference?
        var opt = document.createElement("option");// Yes. Create a row in the selector for this bed chnl.
        // Put the default name (Assign+bedchnlNo) in the text node.
        var chnlTxt = document.createTextNode(bedChnlName(theBedDex, bChnlDex));
        opt.appendChild(chnlTxt);     // Make the chnl's name a part of this dropdown option.
        selRef.appendChild(opt);      // Make this option a part of the dropdown selection.
      }                               // The select reference list has been updated for this SCChnl.
    }                                 // The list of potential reference chnls is complete.
    selRef.style.width = "102px";     // Set a uniform width for the dropdown.
    selRef.style.marginLeft = "10px"; // Put some space between the reference and the type.
    selRef.style.borderRadius = "4px";// Round the corners of the selection box.
    selRef.id = "bedRefChnl";         // Allow access to set this bed chnl's reference.
    if (selType.value == "parent") {  // Is the control type "parent"?
      selRef.disabled = true;         // Yes. A "parent" doesn't have a reference. Disable the refence selector.
      document.getElementById("labelRefSelect").style.color = "lightgray";// Gray the reference label.
    }                                 // The reference selector has the correct enabled state.
    addChnlLoc.appendChild(selRef);   // Make the selection list a part of the modal dialog.
    document.getElementById("addChnlModal").style.display = "block";// Display the modal dialog to select a bed chnl.
  } // End of function addChnl().

  function modifyRefSelect(event) {   // The user clicked to select a chnl's control type.
    var refSelect = document.getElementById("bedRefChnl");// Access the reference chnl selector.
    if(event.srcElement.value == "parent") {// Did the user select a parent control type?
      refSelect.disabled = true;      // Yes. Disable the reference chnl selector. (A parent doesn't have a reference.)
      document.getElementById("labelRefSelect").style.color = "lightgray";// Gray the ref chnl selecter label.
    } else {                          // The user selected a control type which requires a reference chnl.
      refSelect.disabled = false;     // Enable the reference chnl selector.
      refSelect.selectedIndex = "0";
      document.getElementById("labelRefSelect").style.color = "black";// Make the ref chnl selector label black.
    }
  } // End of function modifyRefSelect().

  function editType(event) {          // The user clicked to edit the chnl's control type.
    var refSelect = document.getElementById("editBedRefChnl");// Access the reference chnl selector.
    if(event.srcElement.value == "parent") {// Did the user select a parent control type?
      refSelect.disabled = true;      // Yes. Disable the reference chnl selector. (A parent doesn't have a reference.)
      document.getElementById("editRefLabel").style.color = "lightgray";// Gray the ref chnl selecter label.
    } else {                          // The user selected a control type which requires a reference chnl.
      refSelect.disabled = false;     // Enable the reference chnl selector.
      var bTxt = "bed" + theBedDex;   // Create the bedConfigs key for this bed.
      var bcTxt = "bedChnl" + theChnlDex;// Create the bedConfigs key for this bed chnl.
      var existingRef = bedConfigs[bTxt][bcTxt].bedChnlRefDex;// Get the existing reference.
      if(existingRef == -1) {         // Was there an existing reference?
        refSelect.selectedIndex = "0";// No, the first choice becomes the default coice.
      } else {                        // There is an existing reference.
        refSelect.selectedIndex = existingRef.toString();// Select the existing ref chnl.
      }                               // There reference has been selected.
      document.getElementById("editRefLabel").style.color = "black";// Make the ref chnl selector label is black.
    }                                 // The edit reference selector has been modified for changes to the control type.
  } // End of function editRefSelect().

  function removeChnl() {             // The user wants to remove the given chnl from the given bed.
    var bTxt = "bed" + theBedDex;     // Create the bedConfigs key for this bed.
    var bcTxt = "bedChnl" + theChnlDex;// Create the bedConfigs key for this bed chnl.
    var theBed = bedConfigs[bTxt];    // Access the bed containg the chnl to remove.
    var scChnlDex = theBed[bcTxt].scChnlDex;// Get the scChnlDex of the chnl to remove.
    /*
      If the channel being removed is referenced by another chnl on the bed,
      Display an explanation of why the channel cannot be removed.
      Evaluate each bed chnl. If the bed chnl being evaluated is referenced by the one being
      removed, display a warning and return to the initial beds page. In other words, the
      bedChnlRefDex of the evaluated chnl equals the bedChnlDex of the channel being removed,
      then display a warning and return to the initial beds page.
    */
    // Determine if the channel to be removed is another bedChnl's reference.
    for(var bcDex = 0; bcDex < theBed.noChnls; bcDex++) { // Go through each bedChnl on this bed.
      bedChnlKey = "bedChnl" + bcDex.toString();// Create this bedChnl's key.
      bedChnlRefDex = theBed[bedChnlKey].bedChnlRefDex;// Get its reference.
      // Is this reference the same as this channel to remove?
      if(bedChnlRefDex == theChnlDex) {  // Is this reference and this bedChnlDex the same?
        // Let the user know why this channel cannot be removed.
        var warningDlg = document.getElementById("warningModal");// Use the warningDlg to display a reason.
        var message = "You cannot remove channel " + bcTxt + " from " + theBed.name;// Create
        message += " because it is referenced by channel xxx";// the reason.
        document.getElementById("warningMessage").innerHTML = message;// Set the reason.
        warningDlg.style.display = "block";// Display the warning dialog.
        return;                       // There is nothing else to do here.
      }
    }
    // We want to be really sure the user wants to remove this chnl. So we will ask for confirmation.
    var yesNoDlg = document.getElementById("yesNoModal");// Access the yes/no dialog.
    var message = "Remove channel ";  // The start of the message to confirm the removal of this chnl.
    if(scChnlDex >= 0) {              // Is this chnl defined? (Not assigned at program time.)
      // Yes. Add the chnl's name and bed's name to the confirmation message.
      message += chnls[scChnlDex].name + " from bed " + theBed.name + "?";
    } else {                          // This chnl is assigned at program time.
      message += "Assigned" + (theChnlDex + 1).toString();// Put Assigned and its position in the confirmation message.
      message += " from bed " + theBed.name + "?";// Add the bed name to the message.
    }                                 // The confirmation message has been completed.
    document.getElementById("yesNoMessage").innerHTML = message;// Set the confimation dialog's message.
    yes_action = removeChnl2;         // Set the function to call if the user selects the yes button.
    yesNoDlg.style.display = "block";// Display the confirmation dialog.
  } // End of function removeChnl().

  function removeChnl2() {            // The user wants to remove the given chnl from the given bed.
    var bTxt = "bed" + theBedDex;     // Create the bedConfigs key for this bed.
    // The chnl following the chnl being removed will each be moved forward one place in bedConfigs.
    for (var cDex = theChnlDex; cDex < bedConfigs[bTxt].noChnls - 1; cDex++) {
      var bcTxt = "bedChnl" + cDex;   // Create the bedConfigs key for this bed chnl.
      var bcTxtNext = "bedChnl" + (cDex + 1);// Create the bedConfigs key for this bed chnl.
      bedConfigs[bTxt][bcTxt] = bedConfigs[bTxt][bcTxtNext];// Move the bed chnl forward one position.
    }                                 // All the chnls following the chnl being removed have been moved forward one.
    var bcTxtLast = "bedChnl" + (bedConfigs[bTxt].noChnls - 1);// Create the bed chnl key to the last bed chnl.
    delete bedConfigs[bTxt][bcTxtLast];// Remove the last bed chnl.
    bedConfigs[bTxt].noChnls--;     // Reduce the number of bed channels by one.
    isBedChanged = true;            // There have been changes to the beds by removing a channel.
    pageReset();                    // Recreate the bed table without the chnl which was just removed.
  } // End of function removeChnl2().

  function enterCell(event) {         // The cursor is entering a cell. Highlight it.
    var bgc = event.target.style.backgroundColor;
    if (bgc != "lightgreen" && bgc != "") {
      bColor = event.target.style.backgroundColor;
    }
    event.target.style.backgroundColor = "lightgreen";
    event.currentTarget.style.backgroundColor = "lightgreen";
  }

  function cellLeave(event) {         // Remove the cell's highlighting.
    if(isChnlSelected == true) {
      var dash = event.target.id.indexOf("-");//The bedDex and chnlDex are sperated by a "-" in the id.
      // We need to know the location of the dashes in the id to extract the bedDex and the chnlDex.
      var dash2 = event.target.id.indexOf("-", dash + 1);// Get the location of the second dash.
      var bDex = Number(event.target.id.substr(0, dash));// Extract the bedDex.
      var cDex = Number(event.target.id.substr(dash + 1, dash2 - dash - 1));// Extract the chnlDex.
      if(bDex == theBedDex && cDex == theChnlDex) {
        event.target.style.backgroundColor = "#F3F781";
        return;
      }
    }
    if (event.target.nodeName == "SELECT") {
      event.target.style.backgroundColor = "white";
    } else {
      event.currentTarget.style.backgroundColor = bColor;
    }
  }

  window.onload=drawTable;

//******************************************************************************************
var closeClicked = document.getElementById("closeBedChnl");
closeClicked.onclick = function() {
  document.getElementById("addChnlModal").style.display = "none";
}

var closeEditClicked = document.getElementById("closeEditBedChnl");
closeEditClicked.onclick = function() {
  document.getElementById("editChnlModal").style.display = "none";
}

var cancelAddBedChnlClicked = document.getElementById("cancelAddBedChnlBtn");
cancelAddBedChnlClicked.onclick = function() {
  document.getElementById("addChnlModal").style.display = "none";
}

var addBedChnlBtnClicked = document.getElementById("addBedChnlBtn");// Provide access to the add bed chnl button.
addBedChnlBtnClicked.onclick = function() {// The user clicked add in the bed chnl dialog.
  var bedTxt = "bed" + theBedDex;     // Create the text to access the bed the chnl is being added to.
  var chnlName = document.getElementById("bedChnlSelector").value;// Get the name of the chnl being added.
  var scChnlDex = chnlNameToSCChnlDex(chnlName);// Get the SCChnlDex of the chnl being added.
  // Create a new bed chnl with default values. (1st scChnl, "parent", no reference)
  var newBedChnl = {scChnlDex:"0", bedChnlRefDex:"-1", controlType:"1", assignedChnlDex:"-1" };//
  var ctrlType =  document.getElementById("bedChnlType").value;// Get the chnl's control type name.
  var ctrlTxt = ["none", "parent", "child", "cutoff", "mimic", "cylinder"];// The choices.
  for(var ctrlDex = 0; ctrlDex < ctrlTxt.length; ctrlDex++) {// Go through each control type.
    if (ctrlType == ctrlTxt[ctrlDex]) {// Does this ctrl type name match the selected name?
      newBedChnl.controlType = ctrlDex.toString();// Yes. Set the control type number (ones based).
      break;                          // No need to look farther.
    }                                 // It wasn't this control type.
  }                                   // The control type index has been determined.
  var bedChnlRefDex = -1;             // The default reference index is -1. (there is no reference)
  if (ctrlType != "parent") {         // Is this control type something besides a "parent"?
    var refChnlName =  document.getElementById("bedRefChnl").value;// Yes. Get the reference's name.
    bedChnlRefDex = chnlNameToBedChnlDex(refChnlName);// Get the bed bedChnlRefDex associated with this name. (not the scChnlDex)
  }                                   // The reference's bed chnl index is set.
  newBedChnl.bedChnlRefDex = bedChnlRefDex.toString();// Modify the reference chnl index to be the one just selected.
  var bcTxt = "bedChnl";              // The start of this chnl's bed chnl key.
  if(isAddBedChnl == true) {          // Is this bed chnl being added to the chnls which make up this bed?
    bcTxt += bedConfigs[bedTxt].noChnls;// Yes. Create the text to access the bed having the chnl added.
    bedConfigs[bedTxt].noChnls++;     // Increment the number of chnls on the bed.
  } else {                            // No. This bed chnl is being modified.
    bcTxt += theChnlDex.toString();   // Create the existing key for this bed chnl.
  }                                   // The bed chnl key has been created.
  newBedChnl.scChnlDex = scChnlDex.toString();// Set the scChnlDex of the chnl being added.
  bedConfigs[bedTxt][bcTxt] = newBedChnl;// Add the new chnl to the bed.
  document.getElementById("addChnlModal").style.display = "none";
  document.getElementById("rBedChnlBtn").className = "bedBtnHide";// Hide the remove bed chnl button.
  document.getElementById("rBedBtn").className = "bedBtnHide";// Hide the remove bed button (no bed is selected).
  document.getElementById("aBedChnlBtn").className = "bedBtnHide";// Hide the add chnl btn (the chnl has been added).
  document.getElementById("eBedChnlBtn").className = "bedBtnHide";// Hide the edit chnl btn (the chnl has just been added).
  document.getElementById("cancelBedBtn").className = "bedBtnHide";// Don't want this cancel button.
  document.getElementById("aBedBtn").className = "bedBtnShow bedBtns";// Allow the user to add another bed.
  redrawTable();                      // Recreate the bed table which now includes the new bed.
} // End of the function to add a bed chnl.

var editBedChnlBtnClicked = document.getElementById("editBedChnlBtn");
editBedChnlBtnClicked.onclick = function() {
  var bTxt = "bed" + theBedDex;       // Create the bedConfigs key to access this bed.
  var bcTxt = "bedChnl" + theChnlDex; // Create the bedConfigs key to access this bed chnl.
  var bedChnl = bedConfigs[bTxt][bcTxt]
  var ctrlType =  document.getElementById("editBedChnlType").value;// Get the chnl's control type name.
  var ctrlTxt = ["none", "parent", "child", "cutoff", "mimic", "cylinder"];// The choices.
  for(var ctrlDex = 0; ctrlDex < ctrlTxt.length; ctrlDex++) {// Go through each control type.
    if (ctrlType == ctrlTxt[ctrlDex]) {// Does this ctrl type name match the selected name?
      bedConfigs[bTxt][bcTxt].controlType = ctrlDex.toString();// Yes. Set the control type number (ones based).
      break;                          // No need to look farther.
    }                                 // It wasn't this control type.
  }                                   // The control type index has been determined.
  if(ctrlType == 1) {                 // Is the control type parent?
    var bedChnlRefDex = -1;              // Yes. There is no reference, set it to -1.
  } else {                            // The control type is something besides a "parent"?
    var refChnlName =  document.getElementById("editBedRefChnl").value;// Yes. Get the reference's name.
    bedChnlRefDex = chnlNameToBedChnlDex(refChnlName);// Get the bed bedChnlRefDex associated with this name. (not the scChnlDex)
  }                                   // The reference's bed chnl index is set.
  bedConfigs[bTxt][bcTxt].bedChnlRefDex = bedChnlRefDex.toString();// Modify the reference chnl index to be the one just selected.
  document.getElementById("editChnlModal").style.display = "none";  // Remove the edit bed chnl dialog.
  isBedChanged = true;              // The beds have changed.
  pageReset();                      // Reset the beds page.
} // End of function to edit a bed chnl.

function chnlNameToSCChnlDex(name) {  // Provide the caller with the scChnlDex for the chnl with the given name.
  for(var scDex = 0; scDex < chnls.length; scDex++) {// Go through each chnl on this system.
    var scChnl = chnls[scDex];        // Get this chnls scChnl configuration.
    if(scChnl.name == name) {         // Is this chnl's name the same as the given name?
      return scDex;                   // Provide the caller with the scChnlDex for the chnl with the given name.
    }                                 // The names didn't match.
  }                                   // All the chnls have been tested for the given name.
  return -1;                          // Provide the caller with an invalid scChnlDex.
} // End of function chnlNameToSCChnlDex().

function chnlNameToBedChnlDex(name) {
  var bTxt = "bed" + theBedDex.toString();// Create the key to access the bed containing the chnl.
  var theBed = bedConfigs[bTxt];      // Access the bed containing the chnl.
  // Evaluate each bed chnl.
  for(bedChnlDex = 0; bedChnlDex < theBed.noChnls; bedChnlDex++) {
    // Examine the bedChnl's scChnlDex.
    var bcTxt = "bedChnl" + bedChnlDex;// Create the key to access this bed chnl.
    var theName = bedChnlName(theBedDex, bedChnlDex);// Create the name for this bedChnl.
    // Does this bedChnl's name equal the given name?
    console.log("The given name is " + name);
    var txt = "The name for bed chnl " + bedChnlDex.toString() + " is " + theName
    console.log(txt);
    if(theName == name) {             // Is this bed chnl's name the same as the given name,
      // If so, return the bedChnlDex.
      return bedChnlDex;              // Yes. Provide the caller with the bed chnl index.
    }                                 // This wasn't the chnl identified with the given name.
    // If not, evaluate the next bed Chnl.
  }                                   // End of the loop to evaluate each bedChnl belonging to this bed.
  // If evaluation is complete and no matches have been found, return -1.
  return -1;
} // End of function chnlNameToBedChnlDex().

var cancelBedNameClicked = document.getElementById("cancelBedNameBtn");
cancelBedNameClicked.onclick = function() {
  document.getElementById("bedNameModal").style.display = "none";
}

var bedNameOKClicked = document.getElementById("bedNameOKBtn");
bedNameOKClicked.onclick = function() {
  document.getElementById("bedNameModal").style.display = "none";
  addBedWithName();
}


// When the user clicks in the shaded area outside of the modal, close it.
window.onclick = function(event) {
  var ob = document.getElementById("addChnlModal");
  if (event.target == ob) {
    ob.style.display = "none";
  }
}

var theForm = document.getElementById("bedForm"); // Access the cure configuration form.
theForm.addEventListener("submit", theSubmitFunction);   // Add an event to respond to the user clicking the submit button.

function theSubmitFunction() {                  // The user want to submit bed configurations to the PagesController.
  var j_bedConfigs = JSON.stringify(bedConfigs);// Put the bed configurations into an element
  document.getElementById("theBeds").value = j_bedConfigs;// to tranfer to the PagesController.
  // Convert the session configuration object into a JSON string.
  document.getElementById("peLS").value = JSON.stringify(peLS);
  var actionValue = document.getElementById("theAction");// Get access to the element containg the action.
  if (actionValue.value == "none") {            // Has the action been set?
    actionValue.value = "save";                 // No. Set it to save.
  }
  alert("About to submit using the theSubmitFunction.");
}

function submitForm() {
  document.getElementById("theBeds").value = JSON.stringify(bedConfigs);// Put the bed configurations into a JSON string.
  // Convert the session configuration object into a JSON string.
  document.getElementById("peLS").value = JSON.stringify(peLS);
  document.getElementById("theAction").value = 'none';// Don't do anything special.
  document.getElementById("bedForm").submit();// Submit the form to the server (PagesController).
}

function bedChnlName(bedDex, bedChnlDex) {// Returns the name associated with the given bed (bedDex) and chnl (bedChnlDex).
  var bTxt = "bed" + bedDex;          // Create the bedConfigs key for this bed.
  var bcTxt = "bedChnl" + bedChnlDex; // Create the bedConfigs key for this bed chnl.
  var theBed = bedConfigs[bTxt];      // Access the bed.
  var scChnlDex = Number(theBed[bcTxt].scChnlDex);// Get the scChnlDex of the chnl.
  var name = "Assign" + (Number(bedChnlDex) + 1).toString();// Create this chnl's name as if it is assigned.
  if (scChnlDex >= 0) {               // Is this a defined (not assigned) chnl?
    name = chnls[scChnlDex].name;     // Get this chnl's name.
  }                                   // The name has been created.
  return name;                        // Provide the caller with the name for the given chnl on the given bed.
}

//******************************************************************************************

</script>

@endsection
