
function checkLocalStorage() {        // Test for local storage.
  console.log("Checking local storage.");
  if(peLS['isUsed'] == null) {        // Has the session variable for local storage been set?
    peLS['isUsed'] = 'unknown';       // No. Set it to 'unknown' so that the following lines will evaluate if it is used.
  } // End of condition: Has the session variable for local storage been set?
  if(peLS['isUsed'] == 'unknown') {   // Has local storage been evaluated?
    var colorTxt = localStorage.getItem('PE_Chnl0_Color');// No. Get the color stored for the first chnl.
    console.log("The colorTxt after peLS['isUsed'] = unknow: " + colorTxt);
    if(colorTxt) {                    // Are colors already stored on this user's device?
      console.log("Going to set isUsed to true.");
      peLS['isUsed'] = 'true';        // Yes. Set the session to indicate local storage is used.
    } // End of condition: Local storage is used.
    console.log("peLS determined to = " + peLS['isUsed']);
  } // End of condition: Unknown usage of local storage.
  console.log("unknown isUsed processed.");
  if(peLS['isUsed'] == 'true') {      // Is local storage used?
    loadFromLocalStorage();           // Get the locally stored configuration values.
  } // End of condition: Local storage is used.
  console.log("peLS = " + peLS['isUsed']);
} // End of function checkLocalStorage.

function loadFromLocalStorage() {
  console.log("Loading config from local storage!***3");
  console.log("Chnl 0 color: " + peLS["PE_Chnl_Color"][0]);
  peLS['isUsed'] = 'true';            // Local storage is being used.
  var chnlCount = peLS['PE_Chnl_Color'].length;// Get the number of chnls.
  console.log("Got the chnl length.");
  for(var chnlDex = 0; chnlDex < chnlCount; chnlDex++) {// Go through each chnl.
      // Get this chnl's locally stored color.
      var colorTxt = localStorage.getItem('PE_Chnl' + chnlDex.toString() + '_Color');
      if(colorTxt) {                  // Is there a locally stored color?
        peLS['PE_Chnl_Color'][chnlDex] = colorTxt;// Yes. Put its color in the session variable.;
      } else {                        // The local storage was never set up.
        // Store the session variable which was created by the controller in local storage.
        localStorage.setItem('PE_Chnl' + chnlDex.toString() + '_Color', peLS['PE_Chnl_Color'][chnlDex]);
      } // End of condition: Get/set color from/to local storage.
      if(chnlDex == 0) {
        console.log("peLS color for chnl 0: " + peLS['PE_Chnl_Color'][chnlDex]);
      }
  } // End of looping through all the chnls.
  for(var chnlDex = 0; chnlDex < chnlCount; chnlDex++) {// Go through each chnl.
      // Get this chnl's locally stored plot status.
      var plotTxt = localStorage.getItem('PE_Chnl' + chnlDex.toString() + '_Plot');
      if(plotTxt) {                  // Is there a locally stored plot status?
        peLS['PE_Chnl_Plot'][chnlDex] = plotTxt;// Yes. Put its plot status in the session variable.
      } else {                        // The local storage was never set up.
        // Store the session variable which was created by the controller in local storage.
        localStorage.setItem('PE_Chnl' + chnlDex.toString() + '_Plot', peLS['PE_Chnl_Plot'][chnlDex]);
      } // End of condition: Get/set plot status from/to local storage.
  } // End of looping through all the chnls.
  for(var chnlDex = 0; chnlDex < chnlCount; chnlDex++) {// Go through each chnl.
      // Get this chnl's locally stored program color.
      var colorTxt = localStorage.getItem('PE_Prgm' + chnlDex.toString() + '_Color');
      if(colorTxt) {                  // Is there a locally stored program color?
        peLS['PE_Prgm_Color'][chnlDex] = colorTxt;// Yes. Put its program color in the session variable.
      } else {                        // The local storage was never set up.
        // Store the session variable which was created by the controller in local storage.
        localStorage.setItem('PE_Prgm' + chnlDex.toString() + '_Color', peLS['PE_Prgm_Color'][chnlDex]);
      } // End of condition: Get/set program color from/to local storage.
  } // End of looping through all the chnls.
  for(var chnlDex = 0; chnlDex < chnlCount; chnlDex++) {// Go through each chnl.
      // Get this chnl's locally stored plot status.
      var plotTxt = localStorage.getItem('PE_Prgm' + chnlDex.toString() + '_Plot');
      if(plotTxt) {                  // Is there a locally stored prgm plot status?
        peLS['PE_Prgm_Plot'][chnlDex] = plotTxt;// Yes. Put its plot status in the session variable.
      } else {                        // The local storage was never set up.
        // Store the session variable which was created by the controller in local storage.
        localStorage.setItem('PE_Prgm' + chnlDex.toString() + '_Plot', peLS['PE_Prgm_Plot'][chnlDex]);
      } // End of condition: Get/set plot status from/to local storage.
  } // End of looping through all the chnls.
  var value = localStorage.getItem('PE_TVT_StartTime');// Get the TVT start time.
  if(value) {                         // Is there a locally stored TVT start time?
    peLS['PE_TVT_StartTime'] = value; // Yes. Put the TVT start time in the session variable.
  } else {                            // The local storage was never set up.
    // Store the session variable which was created by the controller in local storage.
    localStorage.setItem('PE_TVT_StartTime', peLS['PE_TVT_StartTime']);
  } // End of condition: Get/set TVT start time from/to local storage.
  value = localStorage.getItem('PE_TVT_Span');// Get the TVT plot span.
  if(value) {                         // Is there a locally stored TVT plot span?
    peLS['PE_TVT_Span'] = value;      // Yes. Put the TVT plot span in the session variable.
  } else {                            // The local storage was never set up.
    // Store the session variable which was created by the controller in local storage.
    localStorage.setItem('PE_TVT_Span', peLS['PE_TVT_Span']);
  } // End of condition: Get/set TVT plot span from/to local storage.
  value = localStorage.getItem('PE_TVT_Max');// Get the TVT maximum grid temperature.
  if(value) {                         // Is there a locally stored TVT maximum grid temperature?
    peLS['PE_TVT_Max'] = value;       // Yes. Put the TVT maximum grid temperature in the session variable.
  } else {                            // The local storage was never set up.
    // Store the session variable which was created by the controller in local storage.
    localStorage.setItem('PE_TVT_Max', peLS['PE_TVT_Max']);
  } // End of condition: Get/set TVT maximum grid temperature from/to local storage.
  value = localStorage.getItem('PE_TVT_Min');// Get the TVT minimum grid temperature.
  if(value) {                         // Is there a locally stored TVT minimum grid temperature?
    peLS['PE_TVT_Min'] = value;       // Yes. Put the TVT minimum grid temperature in the session variable.
  } else {                            // The local storage was never set up.
    // Store the session variable which was created by the controller in local storage.
    localStorage.setItem('PE_TVT_Min', peLS['PE_TVT_Min']);
  } // End of condition: Get/set TVT minimum grid temperature from/to local storage.
  value = localStorage.getItem('PE_TVT_Units');// Get the TVT temperature units.
  if(value) {                         // Is there a locally stored TVT temperature units?
    peLS['PE_TVT_Units'] = value;     // Yes. Put the TVT temperature units in the session variable.
  } else {                            // The local storage was never set up.
    // Store the session variable which was created by the controller in local storage.
    localStorage.setItem('PE_TVT_Units', peLS['PE_TVT_Units']);
  } // End of condition: Get/set TVT temperature units from/to local storage.
  value = localStorage.getItem('PE_Relay_StartTime');// Get the relay history start time.
  if(value) {                         // Is there a locally stored relay history start time?
    peLS['PE_Relay_StartTime'] = value;// Yes. Put therelay history start time in the session variable.
  } else {                            // The local storage was never set up.
    // Store the session variable which was created by the controller in local storage.
    localStorage.setItem('PE_Relay_StartTime', peLS['PE_Relay_StartTime']);
  } // End of condition: Get/set relay history start time from/to local storage.
  value = localStorage.getItem('PE_Relay_Span');// Get the relay history time span.
  if(value) {                         // Is there a locally stored relay history plot span?
    peLS['PE_Relay_Span'] = value;    // Yes. Put the relay history plot span in the session variable.
  } else {                            // The local storage was never set up.
    // Store the session variable which was created by the controller in local storage.
    localStorage.setItem('PE_Relay_Span', peLS['PE_Relay_Span']);
  } // End of condition: Get/set relay history plot span from/to local storage.
  console.log('loadFromLocalStorage relay span: ' + peLS['PE_Relay_Span']);
  var relayCount = peLS['PE_Relay_Color'].length;// Get the number of relays.
  for(var relayDex = 0; relayDex < relayCount; relayDex++) {// Go through each relay.
    // Get this relay's locally stored color.
    var colorTxt = localStorage.getItem('PE_Relay' + relayDex.toString() + '_Color');
    if(colorTxt) {                  // Is there a locally stored relay color?
      peLS['PE_Relay_Color'][relayDex] = colorTxt// Yes. Put its relay color in the session variable.;
    } else {                        // The local storage was never set up.
      // Store the session variable which was created by the controller in local storage.
      localStorage.setItem('PE_Relay' + relayDex.toString() + '_Color', peLS['PE_Relay_Color'][relayDex]);
    } // End of condition: Get/set relay color from/to local storage.
  } // End of looping through all the relays.
  for(var relayDex = 0; relayDex < relayCount; relayDex++) {// Go through each relay.
    // Get this relay's locally stored plot status.
    var plotTxt = localStorage.getItem('PE_Relay' + relayDex.toString() + '_Plot');
    if(plotTxt) {                   // Is there a locally stored relay plot status?
      peLS['PE_Relay_Plot'][relayDex] = plotTxt;// Yes. Put the relay's plot status in the session variable.
    } else {                        // The local storage was never set up.
      // Store the session variable which was created by the controller in local storage.
      localStorage.setItem('PE_Relay' + relayDex.toString() + '_Plot', peLS['PE_Relay_Plot'][relayDex]);
    } // End of condition: Get/set plot status from/to local storage.
  } // End of looping through all the relays.
  console.log('loadFromLocalStorage PE_Relay_Color 0' + peLS['PE_Relay_Color'][0]);
  console.log('loadFromLocalStorage PE_Relay_Plot 0' + peLS['PE_Relay_Plot'][0]);
} // End of function loadFromLocalStorage.
