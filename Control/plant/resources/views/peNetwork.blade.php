@extends('master')
@section('title', 'Network')

@section('content')

<head>

  <!-- http-equiv="refresh" content="3600" causes the page to be refreshed every 3600 seconds (1 hour). -->
  <meta charset="UTF-8" http-equiv="refresh" content="3600">

  <style>

  .pe_title {
    display: flex;
    justify-content: center;
  }

  form {
    padding: 1em;
    display: grid;
    background: #f9f9f9;
    border: 1px solid #c1c1c1;
    margin: 2rem auto 0 auto;
    max-width: 600px;
  }
  form input {
    background: #fff;
    border: 1px solid #9c9c9c;
  }
  form button {
    background: lightgrey;
    padding: 0.5em;
    width: 100%;
    border: 0;
  }
  form button:hover {
    background: gold;
  }


  label {
    padding: 0.2em 0.2em 0.2em 0;
  }

  input {
    padding: 0.2em;
    margin-bottom: 0.5rem;
    border-radius: 4px;
    border-width: thin;
  }

  input:focus {
    outline: 2px solid gold;
  }

  @media (min-width: 400px) {
    form {
      grid-template-columns: 160px 1fr;
      grid-gap: 8px;
    }

    label {
      text-align: right;
      grid-column: 1 / 2;
      color: rgba(0,0,0,0.5);
    }

    input,
    button,
    select {
      grid-column: 2 / 3;
    }
  }
  </style>

</head>

<h1 class="pe_title">Network Configuration</h1>

<form id="sysForm" method="post" enctype="multipart/form-data">

  <!-- Display any errors. -->
  @foreach ($scErrors as $error)
    <p class="alert alert-danger">{{ $error }}</p>
  @endforeach

  <!-- The next line submits the token required by Laravel -->
  <input type="hidden" name="_token" value="{!! csrf_token() !!}">
  <!-- Create a hidden input to hold the action to be performed during a network update. -->
  <input type="hidden" id="netServices" name="netServices">
  <!-- Create a hidden input to hold the pe network configuration during a network update. -->
  <input type="hidden" id="peNetworkConfig" name="peNetworkConfig">
  <!-- Create a hidden input to hold the action to be performed during a bed update. -->
  <input type="hidden" id="theAction" name="theAction" value='none'>
  <!-- Create a hidden input to hold the destination folder to extract the uploaded zip file. -->
  <input type="hidden" id="peDestination" name="peDestination" value='none'>
  <!-- Create a hidden input to hold the local storage status. -->
  <input type="hidden" id="peLS" name="peLS">

  @if (session('status'))
    {{ session('status') }}
  @endif

  <label for="netService">Network Service:</label>
  <select id="netService" name="netService" onChange="serviceChange()"></select>
  <label for="ipAddTxt">IP Address:</label>
  <input type="text" id="ipAddTxt" name="ipAddTxt">
  <label for="maskTxt">Subnet Mask:</label>
  <input type="text" id="maskTxt" name="maskTxt">
  <label for="gatewayTxt">Router:</label>
  <input type="text" id="gatewayTxt" name="gatewayTxt">
  <label for="dns1Txt">DNS Server #1:</label>
  <input type="text" id="dns1Txt" name="dns1Txt">
  <label for="dns2Txt">DNS Server #2:</label>
  <input type="text" id="dns2Txt" name="dns2Txt">
  <button class="peBtns" id="networkConfigBtn" type="button" onclick="setNetworkConfig()">Set Network Configuration</button>
  <label for="peUpLoad">Upload file:</label>
  <input type="file" id="peUpLoad" name="userfile">
  <label for="peDestTBox">Upload Destination:</label>
  <input type="text" id="peDestTBox" name="peDestTBox">
  <button class="peBtns" id="submitBtn" type="submit">Upload File</button>
  <button class="peBtns" id="configDownloadBtn" type="button" onclick="downloadSCConfig()">Download SCConfig.db</button>
  <button class="peBtns" id="clearLocalBtn" type="button" onclick="clearLocalStorage()">Clear Local Storage</button>
  <button class="peBtns" type="button" onclick="location.href='{{ url('/peNetwork')}}'">Cancel</button>
  <button class="peBtns" type="button" onclick="logout()">Log out</button>

</form>


<script>
///////////////////Global/Variables//////////////////////////////////////////
var theForm = document.getElementById("sysForm"); // Access the system configuration form.
theForm.addEventListener("submit", theSubmitFunction);   // Add an event to respond to the user clicking the submit button.
var theNetServices = <?php echo json_encode($netServices); ?>;// Put the network services into a json string.
console.log("The services:");
console.log(theNetServices);
var thePENetConfig = <?php echo json_encode($peNetworkConfig); ?>;// Put the pe network configuration into a json string.
document.getElementById("theAction").value = "none";// The value must not be set to start.
var peLS = <?php echo json_encode($peLS); ?>;// Indicates if it is OK with this user to use local storage.
///////////////End/Global/Variables//////////////////////////////////////////

function theSubmitFunction() {                  // The user wants to submit system configurations to the PagesController.
  var j_netServices = JSON.stringify(theNetServices);// Convert the network services object into a JSON string.
  document.getElementById("netServices").value = j_netServices;// Set the network services for use in regenerating this page.
  thePENetConfig[0].service = document.getElementById("netService").value;// Store the network service.
  thePENetConfig[0].ipAddress = document.getElementById("ipAddTxt").value;// Store the ip address.
  thePENetConfig[0].mask = document.getElementById("maskTxt").value;
  thePENetConfig[0].gatewayAddress = document.getElementById("gatewayTxt").value;
  thePENetConfig[0].dnsServer1 = document.getElementById("dns1Txt").value;
  thePENetConfig[0].dnsServer2 = document.getElementById("dns2Txt").value;
  var j_peNetConfig = JSON.stringify(thePENetConfig);// Convert the pe network configuration object into a JSON string.
  document.getElementById("peNetworkConfig").value = j_peNetConfig;
  document.getElementById("peDestination").value = document.getElementById("peDestTBox").value;
  if(document.getElementById("theAction"). value == "none") {
    document.getElementById("theAction"). value = "upload_File";
  }
  document.getElementById("peLS").value = JSON.stringify(peLS);
}

function logout() {
  document.getElementById("theAction").value = "logout";// The user wants to log out.
  document.getElementById("submitBtn").click();         // Submit the logout request to the server.
}

function downloadSCConfig() {
  document.getElementById("theAction").value = "download_SCConfig";
  document.getElementById("submitBtn").click();         // Refresh the page with the new start hour.
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
  document.getElementById("theAction").value = "clearLocalStorage";
  document.getElementById("submitBtn").click();// Refresh the page with local storage cleared.
} // End of function clearLocalStorage.

function setNetworkConfig() {
  document.getElementById("theAction").value = "set_NetworkConfig";
  document.getElementById("submitBtn").click();         // Refresh the page with the new start hour.
}

function displayConfig() {            // Display the network service configuration.
  var selNetService = document.getElementById('netService');// Access the selector containing the network service.
  for (var i = 0; i < theNetServices.length; i++) {// Go through each possible network service.
    var opt = document.createElement("option");// Create the choice.
    var text = document.createTextNode(theNetServices[i]['service']);// Create its text.
    if (theNetServices[i]['service'] == thePENetConfig[0].service) {// Is this the chnl's control type?
      opt.selected = true;          // Yes. Set its "selected" property.
      displayServiceConfig(i);      // Display the network service configurations for this service.
    }                               // The selected controlType has been evaluated.
    opt.appendChild(text);            // Add the text to the choice.
    selNetService.appendChild(opt);   // Add the choice to the selector.
  } // End of loop to create network service selections.
}

function displayServiceConfig(i) {    // Put each configuration item where it belongs.
  document.getElementById("ipAddTxt").value = theNetServices[i]['ipAddress'];
  document.getElementById("maskTxt").value = theNetServices[i]['mask'];
  document.getElementById("gatewayTxt").value = theNetServices[i]['gatewayAddress'];
  document.getElementById("dns1Txt").value = theNetServices[i]['dnsServer1'];
  document.getElementById("dns2Txt").value = theNetServices[i]['dnsServer2'];
}

function serviceChange() {            // The user wants to change to a different network service.
  var selNetService = document.getElementById('netService');// Access the selector for the network service.
  var selectedService = selNetService.value;// Get the selected network service.
  for(var i = 0; i < theNetServices.length; i++) {// Go through each possible network service.
    if (theNetServices[i]['service'] == selectedService) {// Is this the selected service?
      displayServiceConfig(i);        // Display the existing configuration for this service.
    } // End of condition: This is the selected service, display its config.
  } // End of looping through possible network services.
}

window.onload = displayConfig;


</script>

@endsection
