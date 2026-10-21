<?php

namespace App\Http\Controllers;

use Illuminate\Http\Request;
use App\Http\Requests\PlotTempsRequest;
use App\Http\Requests\CureRequest;
use App\Http\Requests\PlantRequest;
use App\Chnl;
use App\TTSetting;
use App\Program;
use App\Configuration;
use App\Reading;
use App\PmAlert;
use App\PmReply;
use App\Relay;
use App\Input;
use Illuminate\Support\Facades\DB;
use Carbon\Carbon;
use Validator;
use \Datetime;
use \DateTimeZone;
use App\Bed;
use App\peNetwork;
use Illuminate\Support\Facades\Hash;
use App\User;
use Auth;
use App\PMSetting;


class PagesController extends Controller
{

  public function home() {
    $scErrors = array();        // Create an array to hold any error messages. There shouldn't be any.
    $pmSettings = \App\PMSetting::find(1);// Open the pmSettings table to get the last time written by the PM.
    $runTime = $pmSettings->runTime;  // Get the last time written to the DB by the PM.
    $pmVersion = $pmSettings->version;// Get the Plant Manager's version.
    $cRunTime = Carbon::createFromFormat('Y-m-d H:i:s', $runTime);// Create a carbon time for determining the difference in time.
    $cNowTime = Carbon::now('UTC');   // Get the UTC time for right now.
    // Get the difference in time from right now to last time written by the PM.
    $difSeconds = $cNowTime->diffInSeconds($cRunTime);
    $difMinutes = intdiv($difSeconds, 60);// Get the number of minutes for display purposes.
    $secondOfMinute = $difSeconds - $difMinutes * 60;// Calculate the second of the minute for display purposes.
    $runTime = $difMinutes . ' minutes, ' . $secondOfMinute ." seconds";// Create a time diff display string.
    $action = "none";                 // There is no action yet. But it must be defined.
    // The stop button is only enabled if there have been 2 updates after starting the plant manager.
    $stopBtnCount = 9;
    return view('home', compact('runTime', 'pmVersion', 'action', 'stopBtnCount', 'scErrors'));
  }

  public function updateHome(Request $request) {
    $scErrors = array();              // Create an array to hold any error messages.
    $pmSettings = \App\PMSetting::find(1);// Get the PlantManager settings.
    $runTime = $pmSettings->runTime;  // Get the last time written to the DB by the PM.
    $pmVersion = $pmSettings->version;// Get the Plant Manager's version.
    // Create a carbon time for determining the difference in time.
    $cRunTime = Carbon::createFromFormat('Y-m-d H:i:s', $runTime);
    $cNowTime = Carbon::now('UTC');   // Get the UTC time for right now.
    // Get the difference in time from right now to last time written by the PM.
    $difSeconds = $cNowTime->diffInSeconds($cRunTime);
    $difMinutes = intdiv($difSeconds, 60);// Get the number of minutes for display purposes.
    $secondOfMinute = $difSeconds - $difMinutes * 60;// Calculate the second of the minute for display purposes.
    $runTime = $difMinutes . ' minutes, ' . $secondOfMinute ." seconds";// Create a time diff display string.
    $action = $request->action;       // Get the desired action.
    // Get the number of times an update has happened since starting the PM.
    $stopBtnCount = $request->stopBtnCount;
    $pmPath = \App\Configuration::find(1)->pmPath;// Get the path to the Plant Manager.
    if($action == "Update") {         // Is the page being updated?
      // Yes. The stop button is only enabled if there have been 4 updates after
      // starting the plant manager. But the count should not be incremented
      // forever (thus causing an overflow).
      if($stopBtnCount < 9) {         // Is the number of updates since stopping less than 9?
        $stopBtnCount++;              // Yes. Incement the number of updates.
      } // End of condition: Number of updates since stopping is less than 9.
      // exec calls an operating system command. In this casae the command is
      // pgrep with a parameter of PlantManager. If the PlantManager is running,
      // pgrep returns the process id of the PlantManager. If the plantManager
      // is not running, it returns 0.
      $result = exec("pgrep PlantManager");// Get the process id for the PM.
      if($result <= 0) {              // Is the Plant Manager running?
        $stopBtnCount = 3;            // No. Setting $stopBtnCount to three
        // forces the "Stop Plant Manager" button to be disabled. The user
        // is not allowed to turn off the PlantManager when it is not running
      } // End of condition: The PlantManager is not running.
      // Regenerate the home page.
      return view('home', compact('runTime', 'pmVersion', 'action', 'stopBtnCount', 'scErrors'));
    } else if($action == "Start") {     // Does the user want to start the PM?
      $runTime = "0 minutes, 0 seconds";// Yes. This keeps the Start button diabled.
      // The stop button is only enabled if there have been 4 updates after
      // starting the plant manager.
      $stopBtnCount = 0;              // The counter keeps the stop button from
      // being enabled until the PM has had enough time to start.
      // The following is the command which starts the PM. The command includes
      // the path to the PM application. The output of the PM is routed to
      // the file SC.txt in the folder bdat. The & symbols cause the output to
      // be appened to the existing SC.txt file.
      $pmPath = $pmPath . "PlantManager >> /Users/Shared/Sites/bdat/SC.txt&";
      $GLOBALS['pmFile'] = popen($pmPath, 'r');// Start the PM.
    } else if($action == "Stop") {    // Does the user want to stop the PM?
      // The stop button is only disabled after pushing it.
      $stopBtnCount = 0;              // Reset the count to disable the stop btn.
      $error = alertPM("Stop", 0);    // Yes. Tell the Plant Manager to stop.
      array_push($scErrors, "The Plant Manager is being stopped!");// Create an error message.
      if($error != "") {              // Was the Plant Manager stopped?
        array_push($scErrors, $error);// No. Put an error in the error array.
      } // End of condition to see if the PM stopped.
    } else if($action == "Cancel") {// Did the user press the cancel button?
      return redirect('/');         // Yes. The user canceled the action.
    } // End of condition: Handel the users desired action.
    // Regenerate the home page.
    return view('home', compact('runTime', 'pmVersion', 'action', 'stopBtnCount', 'scErrors'));
  } // End of function updateHome.

  public function plant() {
    $scErrors = array();          // Create an array to hold any error messages. There shouldn't be any.
    $chnls = Chnl::all();         // Get the channel data from the SCConfig.db database.
    $readings = Reading::all();   // Get the channels current readings from the SCConfig.db database.
    $id = Auth::id();             // Get the id of the user who is logged in.
    $loggedOn = 'false';          // By default, no one is logged in.
    if($id > 0) {                 // Is there a user logged in?
      $loggedOn = 'true';         // Yes.
    } // End of condition: A user is logged in.
    $peLS = getSessionConfig(); // Get/set the session variable.
    // Display the Plant Manager readings.
  	return view('plant', compact('chnls', 'readings', 'loggedOn', 'peLS', 'scErrors'));
  }

  public function updatePlant(PlantRequest $request) {
    $scErrors = array();        // Create an array to hold any error messages. There shouldn't be any.
    $chnls = Chnl::all();       // Get the channel data from the SCConfig.db database.
    $readings = Reading::all(); // Get the channels current readings from the SCConfig.db database.
    $loggedOn = $request->loggedOn;// Get the flag indicating if a user is logged in.
    $peLS = json_decode($request->peLS, true);// Unpack the existing session configuration.
    if($loggedOn == 'logout') { // Does the user want to log out?
      Auth::logout();           // Yes. Log the user out.
      $loggedOn = 'false';      // Set the flag to indicate no users are logged on.
      $peLS = getSessionConfig(); // Get/set the session variable.
      $peLS['isUsed'] = "unknown";// This user is not logged in. So the default local storage is unknown.
      session(['peLocalStorage' => $peLS['isUsed']]);// Store the unknown local storage state.
    } else {                    // The user is not logging out, just updating.
      session(['peLocalStorage' => $peLS['isUsed']]);
    }// End of condition: The user logged out.
    request()->session()->regenerateToken();// Regenerate the token to prevent a time out.
    // Display the Plant Manager readings.
    return view('plant', compact('chnls', 'readings', 'loggedOn', 'peLS', 'scErrors'));
  }

  public function relayTest() {
    $scErrors = array();                // Create an array to hold the error messages.
    $testRelay = 1;                     // The default test relay is one.
		$desiredState = "on";               // The desired state for the test relay is on.
    $relays = Relay::all();             // Get the relay configurations from the DB.
    $noRelays = count($relays);         // Get the number of relays.
    $action = "none";                   // There is no action yet.
    $peLS = getSessionConfig();         // Get/set the session variable.
    var_dump($peLS['PE_Chnl_Color'][0]);
    if($noRelays == 0) {                // Was there an error reading the DB or no relays on this system?
      array_push($scErrors, "Failed to find any relays!");// Yes. Create an error/warning message.
      // Call the relay test blade.
      return view('relayTest', compact('noRelays', 'testRelay', 'desiredState', 'scErrors', 'action', 'peLS'));
    } // End of condition: There are no relays or there was a DB error.
    // Display the relay test page.
  	return view('relayTest', compact('noRelays', 'testRelay', 'desiredState', 'scErrors', 'action', 'peLS'));
  }

  public function turnOnRelay(Request $request) {
    $scErrors = array();              // Create an array to hold the error messages.
    $relays = Relay::all();           // Get the relays.
    $noRelays = count($relays);       // Get the number of relays.
		$testRelay = $request->testRelay + 1;// The relay to test.
    $action = $request->action;       // Get the action to be performed.
    $peLS = json_decode($request->peLS, true);// Unpack the existing session configuration.
    var_dump($peLS['isUsed']);
    setSessionConfig($peLS);          // Save any configuration changes for this session.
  	$desiredState = "on";             // The default desired state is on.

    // Create the socket to communicate with the Plant Manager.
//      if(!($sock = plantManagerConnect($scErrors))) {  // Was the connection to the Plant Manager established?
//        return view('relayTest', compact('noRelays', 'testRelay', 'desiredState', 's'));// No. The relay test page shows error.
//      } // The Plant Manager connection was established.
    /*
    if($request->submit == "Click to turn on Relay") {
      // The user wants to test a relay. Command the Plant Manager to turn on a relay.
      $command = "RelayTest-" . $request->testRelay;// Send the relay on command and append the relay to turn on.
    } else {                            // The user is done testing the relay.
      $command = "RelayTestTerminate";  // This command terminates the relay test.
    }                                   // The appropriate command has been set.
    $data = plantManagerSend("GetRelays");// Send the command the appropriate command to the Plant Manager.
    */
    if($request->submit == "Click to turn on Relay") {
			// Command the Plant Manager to turn on a relay.

      /*
      $message = "RelayTest-" . $request->testRelay;// Set the relay on command and append the relay to turn on.
      if(!(socket_send ( $sock , $message , strlen($message) , 0))) { // Was the command sent?
        $errorcode = socket_last_error();                 // No. Get the error code.
        $errormsg = socket_strerror($errorcode);          // Turn the error code into a message.
        $errormsg = "Socket error: " . $errormsg;         // Create an error message for a socket with the error description.
        array_push($scErrors, $errormsg);                  // Put the error in the error array.
        return view('relayTest', compact('noRelays', 'testRelay', 'desiredState', 'scErrors'));// The relay test page shows error.
      } // The command to get the relay configuration was sent to the Plant Manager.

      // Now receive the reply from the Plant Manager.
      if(socket_recv ( $sock , $buf , 8096 , MSG_PEEK ) === FALSE) { // Was the reply received?
        $errorcode = socket_last_error();                 // No. Get the error code.
        $errormsg = socket_strerror($errorcode);          // Turn the error code into a message.
        $errormsg = "Socket error: " . $errormsg;         // Create an error message for a socket with the error description.
        array_push($scErrors, $errormsg);                  // Put the error in the error array.
        // The relay test page shows the error.
        return view('relayTest', compact('noRelays', 'testRelay', 'desiredState', 'scErrors'));
      } // The feedback from data has been received from the Plant Manager.
			socket_close($sock);                                // Close the connection to the Plant Manager.
      */

      $error = alertPM("RelayTest", $testRelay - 1);// Tell the Plant Manager to test the relay with the given index.
      if($error != "") {          // Did the Plant Manager activate the relay?
        array_push($scErrors, $error);  // No. Put an error in the error array.
        // The relay test page shows the error.
        return view('relayTest', compact('noRelays', 'testRelay', 'desiredState', 'scErrors', 'action', 'peLS'));
      }                           // The Plant Manager activated the relay.
			$desiredState = 'off';
		} else {

      /*
      $message = "RelayTestTerminate";
      if(!(socket_send ( $sock , $message , strlen($message) , 0))) { // Was the command sent?
        $errorcode = socket_last_error();                 // No. Get the error code.
        $errormsg = socket_strerror($errorcode);          // Turn the error code into a message.
        $errormsg = "Socket error: " . $errormsg;         // Create an error message for a socket with the error description.
        array_push($scErrors, $errormsg);                  // Put the error in the error array.
        // The relay test page shows the error.
        return view('relayTest', compact('noRelays', 'testRelay', 'desiredState', 'scErrors'));
      } // The command to get the relay configuration was sent to the Plant Manager.

      // Now receive the reply from the Plant Manager.
      if(socket_recv ( $sock , $buf , 8096 , MSG_PEEK ) === FALSE) { // Was the reply received?
        $errorcode = socket_last_error();                 // No. Get the error code.
        $errormsg = socket_strerror($errorcode);          // Turn the error code into a message.
        $errormsg = "Socket error: " . $errormsg;         // Create an error message for a socket with the error description.
        array_push($scErrors, $errormsg);                  // Put the error in the error array.
        // The relay test page shows the error.
        return view('relayTest', compact('noRelays', 'testRelay', 'desiredState', 'scErrors'));
      } // The feedback from data has been received from the Plant Manager.
      socket_close($sock);                                // Close the connection to the Plant Manager.
      */

      $error = alertPM("RelayTestTerminate", $testRelay - 1);// Tell the Plant Manager to terminate relay testing.
      if($error != "") {          // Did the Plant Manager activate the relay?
        array_push($scErrors, $error);  // No. Put an error in the error array.
        // The relay test page shows the error.
        return view('relayTest', compact('noRelays', 'testRelay', 'desiredState', 'scErrors', 'action', 'peLS'));
      }                           // The Plant Manager activated the relay.

		}
    // Display the relay test page.
  	return view('relayTest', compact('noRelays', 'testRelay', 'desiredState', 'scErrors', 'action', 'peLS'));
  }

  public function plotTemps() {
    $scErrors = array();      // Create an array to hold the error messages.
    $tvtData = array();       // Create an array to hold all the time/temperature data for all channels being plotted.
    $theChnls = Chnl::all();  // Get all of this system's channel configurations.
    $load = true;             // Signifies, load a TVT config that might be on this user's device.
    // Create an array holding all the TVT settings/configurations perhaps on this user's device.
    $peLS = getSessionConfig(); // Get/set the session variable.
    // Call the plotTemps view to get the TVT settings/configuration on this user's device.
     return view('plotTemps.plotTemps', compact('tvtData', 'scErrors', 'theChnls', 'load', 'peLS'));
  }

  public function updatePlotTemps(PlotTempsRequest $request) {
    $scErrors = array();        // Create an array to hold the error messages.
    $tvtData = array();         // Create an array to hold all the time vs. temperature data for all channels.
    $theChnls = json_decode($request->theChnls, true);// Get the list of chnls which flags the chnls to plot.
    $peLS = json_decode($request->peLS, true);// Unpack the session config.
    setSessionConfig($peLS);    // Save any configuration changes for this session.
    // Create an array containing all the time/temperature data for all channels
    // which are going to be plotted from the database. The array will be in the format used by Chartjs.
    $tvtData = buildTvtData($theChnls, $peLS);
    // Go through all the chnls looking for chnls which are supposed to have their program plotted.
    for($chnlDex = 0; $chnlDex < sizeof($theChnls); $chnlDex++) {// Go through each channel.
      if($peLS['PE_Prgm_Plot'][$chnlDex] == 1) {// Is this chnl's program supposed to be plotted?
        // Yes. Get the Chartjs version of this chnl's program.
        $prgChart = buildTvtPrgChart($chnlDex, $peLS['PE_TVT_Units']);
        array_push($tvtData, $prgChart);// Add it onto all the other arrays that are to be plotted.
      } // End of Condition: This chnl's program has is suppose to be plotted.
    } // End of looping through all chnls.
    $load = false;              // The TVT configuration is not being loaded.
    // Call the plotTemps blade to graph time vs. temperature.
    return view('plotTemps.plotTemps', compact('tvtData', 'scErrors', 'theChnls', 'load', 'peLS'));
  }

  public function cure() {          // Set a cure cycle. Set a program for either a bed or channel.
    $scErrors = array();            // Create an array to hold the error messages.
    $chnls = Chnl::all();           // Get all of this system's channel configurations.
    $theBeds = getBeds();           // Get an array containing each bed's configuration.
    $prgDex = getPrimaryBedChnlDex(0);// Get the primary scChnlDex for the first bed.
    if($prgDex == -1) {             // Is there a primary bed chnl index for the first bed?
      $what = "Chnl-0";             // No, program chnls. (fisrt chnl)
      $prgDex = 0;                  // Set the first chnl as the default to program.
    } else {                        // There are beds.
      $what = "Bed-0";              // Program the first bed.
    }                               // The case of an assigned first chnl has been handled.
    $programs = Program::all();     // Get the programs.
    if (sizeof($programs) <= sizeof($chnls)) { // Are there chnl's with missing programs in the DB?
      createDefaultDBPrograms($programs, $chnls);// Yes. Create and store any missing programs in the DB.
    }                               // A program exists.
    $prgData = DB::connection('sqlite')->table('programs')->where('id', $prgDex + 1)->get();// Get its program from the DB.
    // The DB stores times in UTC. They need to be converted to local time for the user.
    $prgData[0]->timeToStart = UTCToLocalTimeStr($prgData[0]->timeToStart);// Convert the start time to a local time.
    $prgData[0]->timeCreated = UTCToLocalTimeStr($prgData[0]->timeCreated);// Convert the creation time to a local time.
    $prgChart = buildPrgChart($prgDex);// Build the program chart.
    $cureAction = "";                 // This variable is needed for calling the view.
    // Create an array holding the session configuration perhaps on this user's device.
    $peLS = getSessionConfig(); // Get/set the session variable.
    // Call the cure blade to graphically display the program and allow the user to make chages.
    return view('cure', compact('prgData', 'prgChart', 'prgDex', 'chnls', 'cureAction',
                        'theBeds', 'what', 'peLS', 'scErrors'));
  }

  public function updateCure(CureRequest $request) { // Update the cure cycle.
    $scErrors = array();            // Create an array to hold the error messages.
    $prgDex = $request->prgDex;     // Retrieve the posted chnl's index.
    $cureAction = $request->cureAction; // Get the action to be performed.
    $what = $request->what;         // What is the user programming? (bed => Bed-#, Chnl => Chnl-#)
    $chnls = Chnl::all();           // Get the channel configurations.
    $theBeds = json_decode($request->theBeds, true);// Unpack the existing bed configurations.
    $readings = Reading::all();     // Get the current channel temperatures from the SCConfig.db database.
    $thePrgPoints = json_decode($request->thePrgPoints, true);// Unpack the existing program points.
    $peLS = json_decode($request->peLS, true);// Unpack the session config.
    setSessionConfig($peLS);        // Save any configuration changes for this session.

    $prgChart = buildPrgChart($prgDex);// Create an object to chart the program.

    $theTimeZone = \App\Configuration::find(1)->timeZone;// Get the time zone to create times in.

    $dash = strpos($what, "-");     // Parse what is being programmed.
    $prgWhat = substr($what, 0, $dash);// The characters before the dash (Bed or Chnl) represent what is being programmed.
    $bed_Dex = substr($what, $dash + 1, strlen($what) - $dash - 1);// Get the index of what is being programmed.

    if($cureAction == 'chnlChange') {   // Did the user select a new channel?
      // Yes, the user did select a new channel. Get the program for the new channel.
      $prgData = DB::connection('sqlite')->table('programs')->where('id', $prgDex + 1)->get();
      // The DB stores times in UTC. They need to be converted to local time for the user.
      $prgData[0]->timeToStart = UTCToLocalTimeStr($prgData[0]->timeToStart);// Convert the start time to a local time.
      $prgData[0]->timeCreated = UTCToLocalTimeStr($prgData[0]->timeCreated);// Convert the creation time to a local time.
      $prgChart = buildPrgChart($prgDex);// Create an object to chart the program.
    } elseif ($cureAction == 'bedChange') {// The bed being programmed is being chnaged.
      $scChnlDex = getPrimaryBedChnlDex($bed_Dex);// Get the primary scChnl on the bed being changed.
      if($scChnlDex >= 0) {         // Is this a defined scChnl?
        $prgDex = $scChnlDex;       // Yes. This is the index of the prg for this bed.
      } else {                      // No. The bed's primary chnl is undefined.
        $prgDex = 0;                // Set the primary chnl to 0 so the app doesn't crash.
        $scErrors[0] = "The bed's primary chnl is undefined.";// Create an error message.
        // Call the cure blade to graphically display the program and allow the user to make chages.
        return view('cure', compact('prgData', 'prgChart', 'prgDex', 'chnls', 'cureAction',
                            'theBeds', 'what', 'peLS', 'scErrors'));
      }
      // Get the program for the new bed.
      $prgData = DB::connection('sqlite')->table('programs')->where('id', $prgDex + 1)->get();
      // The DB stores times in UTC. They need to be converted to local time for the user.
      $prgData[0]->timeToStart = UTCToLocalTimeStr($prgData[0]->timeToStart);// Convert the start time to a local time.
      $prgData[0]->timeCreated = UTCToLocalTimeStr($prgData[0]->timeCreated);// Convert the creation time to a local time.
      $prgChart = buildPrgChart($prgDex);// Create an object to chart the program.
    } elseif ($cureAction == 'changeType') {// Does the user want to change the program type?
      $prgData = json_decode($request->thePrg, true);// Unpack the existing program.
      $prgChart = json_decode($request->thePrgPoints, true);// Unpack the existing program points.
      if($prgData[0]['type'] == 1 || $prgData[0]['type'] == 3) {// Is the new type a 'Parent' or master with 'Cutoff'?
        $prgChart = array(              // Yes. Create a default program array to
          array(                        // hold this new parent program's time/temp points.
            // The "x" and "y" tags are required by jsChart.
            // "x" contains the time in elapsed hours since the start time and "y" contains the temperature.
            'x' => 0,                   // The elapsed hours since the start time for the first point is always zero.
            'y' => 70                   // Set the default start temperature.
          ),                            // This is the end of the first element.
          array(                        // Set the program's second point.
            'x' => 2,                   // The elapsed time is two hours after the start.
            'y' => 70                   // The program's second temperature is the same as its first.
          )                             // The entire default parent program has been created.
        );  // The array consists of arrays each of which contain elpased hours "x" and a temperature "y".
      } elseif($prgData[0]['type'] == 4) {
        $refChnlDex = $prgData[0]['refChnlDex'];// Get this mimic's reference's index.
        $prgChart = buildPrgChart($refChnlDex);
      } else {                          // This program type is not being changed to a parent.
        $prgChart = array(              // Yes. Create an array to
          array(                        // hold this program's time/temp points.
            // The "x" and "y" tags are required by jsChart.
            // "x" contains the time in elapsed hours since the start time and "y" contains the temperature.
            'x' => 0,                   // The elapsed hours since the start time for the first point is always zero.
            'y' => 0                    // Set the child's start temperature to a default of zero.
          ),                            // This is the end of the first element.
          array(                        // Set the child program's second point.
            'x' => 18,                  // The elapsed time is eighteen hours after the start.
            'y' => 0                    // The program's second temperature is the same as its first.
          )                             // The entire default child program has been created.
        );  // The array consists of arrays which contain elpased hours "x" and a temperature "y".
      }                                 // The default program for the selected type has been created.
    } elseif ($cureAction == 'toggleBedChnl') {// The user is switching between a bed an a channel.
      if($prgWhat == 'Bed') {       // Is a bed is going to be programmed?
        $prgDex = getPrimaryBedChnlDex(0);// Select the primary bed chnl.
        $prgData = DB::connection('sqlite')->table('programs')->where('id', $prgDex + 1)->get();
        // The DB stores times in UTC. They need to be converted to local time for the user.
        $prgData[0]->timeToStart = UTCToLocalTimeStr($prgData[0]->timeToStart);// Convert the start time to a local time.
        $prgData[0]->timeCreated = UTCToLocalTimeStr($prgData[0]->timeCreated);// Convert the creation time to a local time.
        $prgChart = buildPrgChart($prgDex);// Create an object to chart the program.
      } else {                      // A chnl is going to be programmed.
        $prgDex = 0;                // Select the first chnl.
        $prgData = DB::connection('sqlite')->table('programs')->where('id', $prgDex + 1)->get();
        // The DB stores times in UTC. They need to be converted to local time for the user.
        $prgData[0]->timeToStart = UTCToLocalTimeStr($prgData[0]->timeToStart);// Convert the start time to a local time.
        $prgData[0]->timeCreated = UTCToLocalTimeStr($prgData[0]->timeCreated);// Convert the creation time to a local time.
        $prgChart = buildPrgChart($prgDex);// Create an object to chart the program.
      }
    } else {                            // The user didn't select a new channel.
      $prgData = json_decode($request->thePrg, true);// Unpack the existing program.
      $prgChart = $thePrgPoints;        // Set the points to chart. No manipulation is required.
      $errorMsg = validatePrgPoints($thePrgPoints);// Validate the program.
      if(strlen($errorMsg) > 0) {       // Did the program validate?
        $scErrors[0] = $errorMsg;       // No, there was a problem with the program.
        // Display the error associated with the program.
        return view('cure', compact('prgData', 'prgChart', 'prgDex', 'chnls', 'cureAction',
                            'theBeds', 'what', 'peLS', 'scErrors'));
      }                             // The channel's program is ready to plot.
    }                               // The channel's program is ready to plot.
    if($cureAction == 'save') {     // Did the user want to save the program?
      // Yes. Modify the program in the database to reflect the one just entered by the user.
      if($prgWhat == 'Bed') {       // Is the user programming a bed?
        // Yes. Create programs for each chnl on the bed based on their control type and save them.
        // This may also require saving the bed's configuration if there are any assigned chnl.
        createBedChnlPrograms($theBeds, $bed_Dex, $prgData[0], $thePrgPoints, $scErrors, 'save');
      } else {                      // The user wants to save just a chnl's program.
        modifyDBPrg($prgDex, $thePrgPoints, $prgData[0]);// Save the chnl's program.
      }
    } elseif ($cureAction == 'setPrg') {// Does the user want to start or stop a progrm?
      if($prgWhat == 'Bed') {       // Yes. Was the program for a bed?
        // Yes. Create each bed chnl's program and then tell the PM to start each bed chnl's program.
        // This will also require saving the beds configuration if there is an assigned chnl.
        if($prgData[0]['active'] == 0) {// Does the user want to turn off the chnls on the bed?
          deactivateBedChnlPrograms($theBeds, $bed_Dex, $scErrors);// Yes. Deactivate each bed chnl.
        } else {                    // The user wants to activate this bed based on the given program.
          saveBeds($theBeds);       // Save the beds configuration in case an assigned chnl was set.
          createBedChnlPrograms($theBeds, $bed_Dex, $prgData[0], $thePrgPoints, $scErrors, 'setPrg');// Activate bed.
        } // The bed has either been activaed or deactivated.
        if(sizeof($scErrors) != 0) {// Were there any errors?
          // Yes. Display the error.
          return view('cure', compact('prgData', 'prgChart', 'prgDex', 'chnls', 'cureAction',
                              'theBeds', 'what', 'peLS', 'scErrors'));
        }                           // The Plant Manager read the program for each bed chnl.
      } else {                      // The user wants to start a chnl's program.
      // Yes. Modify the program in the database to reflect the one just entered by the user.
      modifyDBPrg($prgDex, $thePrgPoints, $prgData[0]);
      $error = alertPM("SetProgram", $prgDex);// Tell the Plant Manager to read the program.
      if($error != "") {          // Did the Plant Manager read the program?
          array_push($scErrors, $error);  // No. Put an error in the error array.
          // Display the error.
          return view('cure', compact('prgData', 'prgChart', 'prgDex', 'chnls', 'cureAction',
                              'theBeds', 'what', 'peLS', 'scErrors'));
        }                           // The Plant Manager read the program.
      }
    } elseif ($cureAction == 'new') { // The user wants to reset the program.
      extract(getDefaultDBProgram()); // Get the default program.
      // The program points will be ignored and the following will be used for program points.
      $theTemp = $readings[$prgDex]['valueF'];// Get the selected channel's temperature.
      if ($theTemp == 'Open') {       // Is the temparture 'Open'?
        $theTemp = (float)0.0;        // Yes. Set the programs temperature very low so heat does not come on.
      }                               // The temperature is set.
      $prgChart = array(              // Create a default program array to
        array(                        // hold this new parent program's time/temp points.
          // The "x" and "y" tags are required by jsChart.
          // "x" contains the time in elapsed hours since the start time and "y" contains the temperature.
          'x' => 0,                   // The elapsed hours since the start time for the first point is always zero.
          'y' => $theTemp             // Set the default start temperature.
        ),                            // This is the end of the first element.
        array(                        // Set the program's second point.
          'x' => 2,                   // The elapsed time is two hours after the start.
          'y' => $theTemp             // The program's second temperature is the same as its first.
        )                             // The entire default parent program has been created.
      );  // The array consists of arrays each of which contain elpased hours "x" and a temperature "y".
      $theDBPrg->cutoffTemp = $prgData[0]['cutoffTemp'];// Dont' change the cutoffTemp for a new program.
      $theDBPrg->maintenanceTemp = $prgData[0]['maintenanceTemp'];// Dont' change the maintenanceTemp for a new program.
      $prgData[0] = $theDBPrg;        // Put the program in the array used by the cure blade.
      $prgData[0]->timeToStart = UTCToLocalTimeStr($prgData[0]->timeToStart);// Convert the start time to a local time.
      $prgData[0]->timeCreated = UTCToLocalTimeStr($prgData[0]->timeCreated);// Convert the creation time to a local time.
    }                                 // The desired cure action has been completed.
    // Call the cure blade to graphically display the program and allow the user to make chages.
    return view('cure', compact('prgData', 'prgChart', 'prgDex', 'chnls', 'cureAction',
                        'theBeds', 'what', 'peLS', 'scErrors'));
  } // End of function updateCure.

  public function plotRelays() {
    $scErrors = array();              // Create an array to hold the error messages.
    // Define default settings in order to query the user's device for the actual settings.
    $relays = Relay::all();           // Get the relays.
    $rhData = "";                     // updatePlotRelays will create the relay history data.
    // Create an array holding the session configuration perhaps on this user's device.
    $peLS = getSessionConfig();       // Get/set the session variable.
    $rhData = buildRHData($relays, $peLS);// Build an array formatted to plot the relay history(s).
    // Generate the page in order to get the settings stored on the user's device.
    // The settings are accessed usign javascript on the client (the user's device).
    return view('plotRelays', compact('relays', 'rhData', 'scErrors', 'peLS'));
  }

  public function updatePlotRelays(Request $request) {// The user has changed something for plotting relays.
    $scErrors = array();              // Create an array to hold the error messages.
    $rhData = array();                // Create an array to hold all the relay history data for all relays being plotted.
    $relays = json_decode($request->theRelays, true);// Get the list of chnls which flags the chnls to plot.
    $peLS = json_decode($request->peLS, true);// Unpack the session config.
    setSessionConfig($peLS);          // Save any configuration changes for this session.
    // Create an array containing all the time/percetage-on data for all relays
    // which are going to be plotted. The array will be in the format used by Chartjs.
    $rhData = buildRHData($relays, $peLS);
    return view('plotRelays', compact('relays', 'rhData', 'scErrors', 'peLS'));// Display the percetage-on chart.
  } // End of function updatePlotRelays.

  public function beds() {
    $scErrors = array();          // Create an array to hold the error messages.
    $chnls = Chnl::all();         // Access the DB channels.
    $theBeds = getBeds();         // Get an array containing each bed's configuration.
    $peLS = getSessionConfig();     // Get/set the session variable.
    return view('beds', compact('theBeds', 'chnls', 'scErrors', 'peLS'));// Show the beds.
  }

  public function updateBeds(Request $request) {
    $scErrors = array();          // Create an array to hold the error messages.
    $chnls = Chnl::all();         // Access the DB channels.
    $theBeds = json_decode($request->theBeds, true);// Unpack the existing bed configurations.
    $theAction = $request->theAction; // Get the action to be performed.
    if ($theAction == "save") {
      saveBeds($theBeds);
    }
    // 'unknown' => not assigned yet, 'true' => local storage is used, 'false' => no local storage.
    $peLS = json_decode($request->peLS, true);// Unpack the existing configurations.
    setSessionConfig($peLS);          // Save any configuration changes for this session.
    // Show the beds page.
    return view('beds', compact('theBeds', 'chnls', 'scErrors', 'peLS'));
  }

  public function chnls() {
    $scErrors = array();              // Create an array to hold the error messages.
    $theChnls = Chnl::all();          // Access the DB channels.
    $inputs = Input::all();           // Access the DB inputs.
    $relays = Relay::all();           // Access the DB relays.
    $theBeds = getBeds();             // Get an array containing each bed's configuration.
    $pmSettings = \App\PMSetting::find(1);// Open the pmSettings table to get the last time written by the PM.
    $runTime = $pmSettings->runTime;  // Get the last time written to the DB by the PM.
    $version = $pmSettings->version;  // Get the Plant Manager's version.
    $runTime = timeDifString();       // Get the amount of time since the PM last updated in a string.
    // The stop button is only enabled if there have been 3 updates after starting the plant manager.
    $stopCount = 9;                   // Allow the user to stop the PM.
    $action = 'none';                 // There is no action yet. The user hasn't done anything.
    $peLS = getSessionConfig();     // Get/set the session variable.
    // Show the Channels page.
    return view('chnls', compact('theChnls', 'inputs', 'relays', 'theBeds', 'runTime',
                  'version', 'stopCount', 'action', 'peLS', 'scErrors'));
  }

  public function updateChnls(Request $request) {
    $scErrors = array();              // Create an array to hold the error messages.
    $theChnls = json_decode($request->theChnls, true);// Unpack the existing bed configurations.
    $inputs = Input::all();           // Access the DB inputs.
    $relays = Relay::all();           // Access the DB relays.
    $pmPath = \App\Configuration::find(1)->pmPath;// Get the path to the Plant Manager.
    $pmSettings = \App\PMSetting::find(1);// Open the pmSettings table to get the last time written by the PM.
    $runTime = $pmSettings->runTime;  // Get the last time written to the DB by the PM.
    $version = $pmSettings->version;  // Get the Plant Manager's version.
    $runTime = timeDifString();       // Get the amount of time since the PM last updated in a string.
    $theBeds = json_decode($request->theBeds, true);// Unpack the existing bed configurations.
    $action = $request->action;       // Get the action to be performed.
    // 'unknown' => not assigned yet, 'true' => local storage is used, 'false' => no local storage.
    $peLS = json_decode($request->peLS, true);// Unpack the existing configurations.
    setSessionConfig($peLS);          // Save any configuration changes for this session.
    //$request->session()->regenerateToken();
    if ($action == "save") {          // Does the user want to save the new chnl config?
      saveChnls($theChnls);           // Yes. Save the new chnl config.
      saveBeds($theBeds);             // Save the bed configs just in case their chnls shifted.
      //**** Stop the Plant Manager. *******************************************
      if(exec("pgrep PlantManager") > 0) { // Is the Plant Manager running?
        $stopCount = 0;                 // Yes. Reset the count to disable the stop btn.
        $error = alertPM("Stop", 0);    // Yes. Tell the Plant Manager to stop.
        sleep(15);                      // Wait fifteen seconds to allow the PM to stop.
        $runTime = "0 minutes, 0 seconds";// Yes. This keeps the Start button diabled.
      } // End of condition: The Plant Manager was running.
      //**** Confirm the Plant Manager has stopped. ****************************
      // exec calls an operating system command. In this casae the command is
      // pgrep with a parameter of PlantManager. If the PlantManager is running,
      // pgrep returns the process id of the PlantManager. If the plantManager
      // is not running, it returns 0.
      while(exec("pgrep PlantManager") > 0) { // Is the Plant Manager running?
        sleep(2);                     // Yes. Wait2 seconds more for the PM to finish stopping.
      } // End of condition: The Plant Manager is running.
      //**** Start the Plant Manager. ******************************************
      // The following is the command which starts the PM. The command includes
      // the path to the PM application. The output of the PM is routed to
      // the file SC.txt in the folder bdat. The & symbols cause the output to
      // be appened to the existing SC.txt file.
      $pmPath = $pmPath . "PlantManager >> /Users/Shared/Sites/bdat/SC.txt&";
      $GLOBALS['pmFile'] = popen($pmPath, 'r');// Start the PM.
      // Display the channels page.
      return view('chnls', compact('theChnls', 'inputs', 'relays', 'theBeds', 'runTime',
                    'version', 'stopCount', 'action', 'peLS', 'scErrors'));
    } // End of condition "save".
    if($action == "UpdateRevert") {   // The user wants to revert to the original settings.
      $theChnls = Chnl::all();        // Get the DB channels configuration.
    } // End of condition: The user wants to revert to the original configuration.
    // Update the Channels page.
    // Get the number of times an update has happened since starting the PM.
    $stopCount = $request->stopCount; // After 3 updates, the user is allowed to stop the PM.
    $updateAction = substr($action, 0, 6);// See if the action is an Update action.
    if($updateAction == "Update") {// Is the page being updated?
      // Yes. The stop button is only enabled if there have been 4 updates after
      // starting the plant manager. But the count should not be incremented
      // forever (thus causing an overflow). "Update" is just a plain update. "UpdateSave"
      // is an update with the save and cancel buttons displayed.
      if($stopCount < 9) {            // Is the number of updates since stopping less than 9?
        $stopCount++;                 // Yes. Incement the number of updates.
      } // End of condition: Number of updates since stopping is less than 9.
      // exec calls an operating system command. In this casae the command is
      // pgrep with a parameter of PlantManager. If the PlantManager is running,
      // pgrep returns the process id of the PlantManager. If the plantManager
      // is not running, it returns 0.
      $result = exec("pgrep PlantManager");// Get the process id for the PM.
      if($result <= 0) {              // Is the Plant Manager running?
        $stopCount = 0;               // No. Setting $stopBtnCount to zero
        // forces the "Stop Plant Manager" button to be disabled. The user
        // is not allowed to turn off the PlantManager when it is not running
      } // End of condition: The PlantManager is not running.
      // Regenerate the Channels page.
      if($action == "Update120") {    // Has the chnls page been updating without user action for about 30 minutes?
        return redirect('chnls');     // Yes. Redirect to the chnls page to try to avoid a 419.
      } // End of condition: the chnls page has been updating for about 30 minutes without user action.
      // Regenerate the Channels page.
      return view('chnls', compact('theChnls', 'inputs', 'relays', 'theBeds', 'runTime',
                    'version', 'stopCount', 'action', 'peLS', 'scErrors'));
    } else if($action == "Start") {   // Does the user want to start the PM?
      $runTime = "0 minutes, 0 seconds";// Yes. This keeps the Start button diabled.
      // The stop button is only enabled if there have been 4 updates after
      // starting the plant manager.
      $stopCount = 0;                 // The counter keeps the stop button from
      // being enabled until the PM has had enough time to start.
      // The following is the command which starts the PM. The command includes
      // the path to the PM application. The output of the PM is routed to
      // the file SC.txt in the folder bdat. The & symbols cause the output to
      // be appened to the existing SC.txt file.
      $pmPath = $pmPath . "PlantManager >> /Users/Shared/Sites/bdat/SC.txt&";
      $GLOBALS['pmFile'] = popen($pmPath, 'r');// Start the PM.
    } else if($action == "Stop") {    // Does the user want to stop the PM?
      // The stop button is only disabled after pushing it.
      $stopCount = 0;                 // Reset the count to disable the stop btn.
      $error = alertPM("Stop", 0);    // Yes. Tell the Plant Manager to stop.
      array_push($scErrors, "The Plant Manager is being stopped!");// Create an error message.
      if($error != "") {              // Was the Plant Manager stopped?
        array_push($scErrors, $error);// No. Put an error in the error array.
      } // End of condition to see if the PM stopped.
    } else if($action == "Cancel") {  // Did the user press the cancel button?
      return redirect('/');           // Yes. The user canceled the action.
    } // End of condition: Handel the users desired action.
    // Regenerate the Channels page.
    return view('chnls', compact('theChnls', 'inputs', 'relays', 'theBeds', 'runTime',
                  'version', 'stopCount', 'action', 'peLS', 'scErrors'));

//    return view('chnls', compact('chnls', 'inputs', 'relays', 'theBeds', 'scErrors'));

// !!!!
} // End of function updateChnls.

  public function peNetwork() {       // The user (Products Engineering) wants to configure the network.
    $user = Auth::user();
    $id = Auth::id();
    if($id != 1) {
      my_alert("Insufficient permissions!");
      return redirect('login');// This does not work.
    }
    $scErrors = array();              // Create an array to hold the error messages.
    $allNetServices = array();        // Create an array to hold the computer's network services.
    $netServices = array();           // Create an array to hold the network services which can be used by the controller.
    $peNetworkConfig = peNetwork::all();// Get the network configuration from the DB.
    // "exec" executes the given command. In this case sudo networksetup -listallnetworkservices.
    exec("sudo networksetup -listallnetworkservices", $allNetServices, $retVal);// Get all possible network services on this PC.
    for($dex = 1; $dex < sizeof($allNetServices); $dex++) {// Go through all the network services.
      $netInfo = "";                  // Clear $netInfo to hold new network service information (don't want to append).
      exec("sudo networksetup -getinfo '" . $allNetServices[$dex] . "'", $netInfo, $retVal);// Get info for this service.
      // The IP address is in the second item if this network service is able to connect to a controller.
      $ipData = substr($netInfo[1], 0, 10);// Get the first ten characters of the second item.
      if($ipData != "IP address") {   // Did the characters indicate it was not an IP address (only IP address is allowed)?
        continue;                     // Skip to the next network service. This one won't work.
      } // End of condition: Second config item didn't contain an IP address.
      $the_service = $allNetServices[$dex];// Get the network service name.
      $the_ipAddress = substr($netInfo[1], 12);// Get the network service IP address.
      $the_mask = substr($netInfo[2], 13);// Get the network service subnet mask.
      $the_gatewayAddress = substr($netInfo[3], 8);// Get the network service router address.
      $dnsInfo = "";                  // Clear $dnsInfo to hold new DNS information (don't want to append).
      exec("sudo networksetup -getdnsservers '" . $the_service . "'", $dnsInfo, $retVal);// Get DNS info for this service.
      $the_dnsServer1 = "";           // Prepare an empty address for DNS server #1.
      $the_dnsServer2 = "";           // Prepare an empty address for DNS server #2.
      if(sizeof($dnsInfo) > 0) {      // Was anything returned for the DNS servers?
        $the_dnsServer1 = $dnsInfo[0];// Yes. Copy the first item (first DNS address).
        if(sizeof($dnsInfo) > 1) {    // Is there a second DNS address?
          $the_dnsServer2 = $dnsInfo[1];// Yes. Copy the second item (second DNS address).
        } // End of condition: There was a second DNS address.
      } // End of condition: There is at least one DNS address.
      $theNetService = array(         // Create an object containing the network service configuration.
        "service" => $the_service,    // Store the service name.
        "ipAddress" => $the_ipAddress,// Store the service IP address.
        "mask" => $the_mask,          // Store the service subnet mask.
        "gatewayAddress" => $the_gatewayAddress,// Store the service router address.
        "dnsServer1" => $the_dnsServer1,// Store the service first DNS server address.
        "dnsServer2" => $the_dnsServer2// Store the service second DNS server address.
      );                              // The network service object is complete.
      array_push($netServices, $theNetService);// Add it to the array of acceptable network services.
    } // End of looping through all network services.
    $peLS = getSessionConfig();     // Get/set the session variable.
    return view('peNetwork', compact('netServices', 'peNetworkConfig', 'peLS', 'scErrors'));// Display the network config.
  } // End of function peNetwork.

  public function updatePENetwork(Request $request) {
    $scErrors = array();            // Create an array to hold the error messages.
    $netServices = json_decode($request->netServices, true);// Unpack the existing network services.
    $peNetworkConfig = json_decode($request->peNetworkConfig, true);// Unpack the existing pe network configuration.
    $theAction = $request->theAction; // Get the action to be performed.
    $peDestination = $request->peDestination;// Get the destination folder for an upload.
    $peLS = json_decode($request->peLS, true);// Unpack the existing configurations.
    setSessionConfig($peLS);
//    session(['peLocalStorage' => $peLS['isUsed']]);// The key peLocalStorage is associated with $peLS['isUsed'].
//    session(['PE_Chnl_Color' => $peLS['PE_Chnl_Color']]);// The key PE_Chnl_Color is associated with $peLS['PE_Chnl_Color'].
    if($theAction == "download_SCConfig") {// The SCConfig.db is to be downloaded.
      downloadSCConfig();             // Download SCConfig.db to the default download folder.
      return view('peNetwork', compact('netServices', 'peNetworkConfig', 'peLS', 'scErrors'));// Display the network config.
    } // End of condition: The user wants to download the SCConfig.db file.
    if($theAction == "set_NetworkConfig") {
      if (filter_var($peNetworkConfig[0]["ipAddress"], FILTER_VALIDATE_IP) == FALSE) {// Is the ipAddress valid?
        $scErrors[0] = "The IP Address is invalid!";// No. Create an error message.
        return view('peNetwork', compact('netServices', 'peNetworkConfig', 'peLS', 'scErrors'));// Display the network config.
      } // The ipAddress is valid.
      if (filter_var($peNetworkConfig[0]["mask"], FILTER_VALIDATE_IP) == FALSE) {// Is the Subnet Mask valid?
        $scErrors[0] = "The Subnet Mask is invalid!";// No. Create an error message.
        return view('peNetwork', compact('netServices', 'peNetworkConfig', 'peLS', 'scErrors'));// Display the network config.
      } // The Subnet Mask is valid.
      if (filter_var($peNetworkConfig[0]["gatewayAddress"], FILTER_VALIDATE_IP) == FALSE) {// Is the Router valid?
        $scErrors[0] = "The Router Address is invalid!";// No. Create an error message.
        return view('peNetwork', compact('netServices', 'peNetworkConfig', 'peLS', 'scErrors'));// Display the network config.
      } // The Router is valid.
      if (filter_var($peNetworkConfig[0]["dnsServer1"], FILTER_VALIDATE_IP) == FALSE) {// Is the DNS Server 1 valid?
        $scErrors[0] = "The DNS Server #1 is invalid!";// No. Create an error message.
        return view('peNetwork', compact('netServices', 'peNetworkConfig', 'peLS', 'scErrors'));// Display the network config.
      } // The DNS Server 1 is valid.
      if (filter_var($peNetworkConfig[0]["dnsServer2"], FILTER_VALIDATE_IP) == FALSE) {// Is the DNS Server 2 valid?
        $scErrors[0] = "The DNS Server #2 is invalid!";// No. Create an error message.
        return view('peNetwork', compact('netServices', 'peNetworkConfig', 'peLS', 'scErrors'));// Display the network config.
      } // The DNS Server 2 is valid.
      // Check for any address conflicts.
      // Check that $peNetworkConfig does not have the same IP address as any gatewayServer or the IP adresses of any
      // other network service.
      $dexOfServiceToChange = -1;     // The index of the network service being changed.
      for($servDex = 0; $servDex < sizeof($netServices); $servDex++) {// Go through each usable network service.
        // Does the modified server's IP address match this potential server's gateway address?
        if($peNetworkConfig[0]['ipAddress'] == $netServices[$servDex]['gatewayAddress']) {
          $scErrors[0] = "The IP address, " . $peNetworkConfig[0]['ipAddress'];// Yes. This cannot be allowed.
          $scErrors[0] .= " is already being used by a Router!";// Create an error message.
          return view('peNetwork', compact('netServices', 'peNetworkConfig', 'peLS', 'scErrors'));// Display the error.
        }
        if($netServices[$servDex]['service'] == $peNetworkConfig[0]['service']) {// Is this the selected service?
          $dexOfServiceToChange = $servDex;// Save the index of the network service being changed.
          continue;                   // Nothing else needs to be checked.
        } // End of condition: This is the selected service.
        // Does the modified server's IP address match this potential server's IP address?
        if($peNetworkConfig[0]['ipAddress'] == $netServices[$servDex]['ipAddress']) {
          $scErrors[0] = "The IP address, " . $peNetworkConfig[0]['ipAddress'];// Yes. This cannot be allowed.
          $scErrors[0] .= " is already being used by server ";// Create an error
          $scErrors[0] .= $netServices[$servDex]['service'] . "!";// message.
          return view('peNetwork', compact('netServices', 'peNetworkConfig', 'peLS', 'scErrors'));// Display the error.
        } // End of condition: The IP address is being used by another network service.
      } // End of looping through the potential network servers.
      $message = peSetNetwork($netServices, $peNetworkConfig);// Set the network server's configuration.
      if($message != "") {            // Was there a problem?
        $scErrors[0] = $message;      // Set the error.
        return view('peNetwork', compact('netServices', 'peNetworkConfig', 'peLS', 'scErrors'));// Display the error.
      } // End of condition: There was an error setting the network service configuration.
      $netServices[$dexOfServiceToChange]['ipAddress'] = $peNetworkConfig[0]['ipAddress'];
      $netServices[$dexOfServiceToChange]['mask'] = $peNetworkConfig[0]['mask'];
      $netServices[$dexOfServiceToChange]['gatewayAddress'] = $peNetworkConfig[0]['gatewayAddress'];
      $netServices[$dexOfServiceToChange]['dnsServer1'] = $peNetworkConfig[0]['dnsServer1'];
      $netServices[$dexOfServiceToChange]['dnsServer2'] = $peNetworkConfig[0]['dnsServer2'];
      return view('peNetwork', compact('netServices', 'peNetworkConfig', 'peLS', 'scErrors'));// Display the network config.
    } // End of condition: Set the network service configuration.
    if($theAction == "logout") {    // Does the user want to log out?
      Auth::logout();               // Log out so an unauthorized user can't get on this page.
      $peLS = getSessionConfig(); // Get/set the session variable.
      $peLS['isUsed'] = "unknown";  // This user is not logged in. So the default local storage is unknown.
      session(['peLocalStorage' => $peLS['isUsed']]);// Store the unknow local storage state.
      return redirect('plant');     // Go to the Plant page.
    } // End of condition: The user wants to log out.
    if($theAction == "clearLocalStorage") {// Does the user want to clear local storage?
      // This was done on the client side. The page is just being updated here.
      return view('peNetwork', compact('netServices', 'peNetworkConfig', 'peLS', 'scErrors'));// Display the network config.
    } // End of condition: The user cleared local storage.
    // The only action remaining is to upload a zip file to the customer's computer in the desired
    // destination folder, and from there extract the contents of the zip files.
    // PHP basename returns the file name without its path.
    $fileName = basename($_FILES['userfile']['name']);// Get the original name of the file on the client machine.
    if($fileName != "") {             // Is there a file name?
      ini_set('max_execution_time', 120);
      $targetDir = $peDestination;    // Yes. Specify the path to extract the zip file on the server.
      // The file will first be copied to /Users/Shared/Sites/ because it has the correct
      // permissions to accept a new file. From /Users/Shared/Sites/ the file will be unzipped to
      // the desired folder ($targetDir).
      $uploadDir = "/Users/Shared/Sites/";// The folder where all files will be uploaded.
      $targetFile = $uploadDir . $fileName;// Create the complete file name including the path on the server.
      // $_FILES['userfile']['tmp_name'] contains the temporary filename of the uploaded file
      // stored on the server.
      // move_uploaded_file — Moves an uploaded file to a new location. In our case to /Users/Shared/Sites/.
      if (move_uploaded_file($_FILES['userfile']['tmp_name'], $targetFile) == FALSE) {// Was the move successful?
        // No. Create an error message.
        $scErrors[0] = "Failed to move the uploaded file," . $targetFile . " on the customer's computer";
        return view('peNetwork', compact('netServices', 'peNetworkConfig', 'peLS', 'scErrors'));// Display the error.
      } // End of condition: The uploaded file failed to transfer to the desired folder.
      unzipFile($targetFile, $targetDir);// Unzip, extract, the contents of the uploaded file.
    } // End of condition: There is a file (since it was selected using a file browser).
  } // End of updatePENetwork().

} // End of class PagesController.

function downloadSCConfig() {         // The user wants to download SCConfig.db from the customer's computer.
  ob_start();                         // Turn output buffering on (the file could be large).
  $file = "/Users/Shared/Sites/bdat/SCConfig.db";// This is the complete file name on the user's computer.
  if(file_exists($file)) {            // Make sure SCConfig.db exists.
    // Send a raw HTTP header that indicates a file is to be transferred. This seems like black magic. But it works.
    header('Content-Description: File Transfer');
    header('Content-Type: application/octet-stream');
    header('Content-Disposition: attachment; filename='.basename($file));
    header('Content-Transfer-Encoding: binary');
    header('Expires: 0');
    header('Cache-Control: must-revalidate');
    header('Pragma: public');
    header('Content-Length: ' . filesize($file));
    ob_clean();                       // Discard any existing contents of the output buffer.
    readfile($file);                  // Read the file and write it to the output buffer.
  } // End of condition: SCConfig.db exists.
}

function unzipFile($fileName, $peDestination) { // On the remote computer,
  // unzip $fileName to the folder $peDestination.
  // Start of the unix unzip command. -q => no ouput, -o => overwrite existing files,
  // -D => suppress restoration of timestamps, -K => don't change the attributes.
  $unzipCmnd = "sudo unzip -q -o -D -K ";
  $unzipCmnd .= $fileName;            // Add the file (including path) to extract.
  $unzipCmnd .= " -d " . $peDestination;// -d => the destination folder ($peDestination) for the extraction.
  exec($unzipCmnd, $result, $retVal); // Execute the unzip cmnd.
}

function peSetNetwork($netServices, $peNetworkConfig) {// This function sets the network on this computer.
  $service = $peNetworkConfig[0]["service"];// Transfer network variables
  $ipAddress = $peNetworkConfig[0]["ipAddress"];// to local variables.
  $mask = $peNetworkConfig[0]["mask"];// Get the subnet mask.
  $gatewayAddress = $peNetworkConfig[0]["gatewayAddress"];// Get the router address.
  $dnsServer1 = $peNetworkConfig[0]["dnsServer1"];// Get the first DNS server.
  $dnsServer2 = $peNetworkConfig[0]["dnsServer2"];// Get the second DNS server.
  $peNetCon = \App\peNetwork::find(1);// Get the peNetwork table.
  $peNetCon->service = $service;      // Transfer the network service to the DB record.
  $peNetCon->ipAddress = $ipAddress;  // Transfer the IP Address to the DB record.
  $peNetCon->mask = $mask;            // Transfer the Subnet Mask to the DB record.
  $peNetCon->gatewayAddress = $gatewayAddress;// Transfer the Router to the DB record.
  $peNetCon->dnsServer1 = $dnsServer1;// Transfer the DNS Server 1 to the DB record.
  $peNetCon->dnsServer2 = $dnsServer2;// Transfer the DNS Server 2 to the DB record.
  $peNetCon->save();                  // Save the updated record to the DB.
  // Create the networksetup command to set the IP Address, Subnet Mask, and Router Address.
  $networkCmnd = "sudo networksetup -setmanual \"" . $peNetCon->service . "\" ";
  $networkCmnd .= $peNetCon->ipAddress . " ";// Add the IP Address.
  $networkCmnd .= $peNetCon->mask . " ";// Add the Subnet Mask.
  $networkCmnd .= $peNetCon->gatewayAddress;// Add the Router Address.
  exec($networkCmnd, $result, $retVal);// Execute the networksetup cmnd.
  if($retVal == 1) {                  // Does the return value indicate failure?
    $scErrors[0] = "The computer failed to accept " . $networkCmnd . "!";// Yes. Create an error message.
    return view('peNetwork', compact('netServices', 'peNetworkConfig', 'peLS', 'scErrors'));// Display the network config.
  } // The networksetup cmnd succeeded.
  // Create the networksetup command to set the DNS Servers.
  $networkCmnd = "sudo networksetup -setdnsservers \"" . $peNetCon->service . "\" ";
  $networkCmnd .= $peNetCon->dnsServer1 . " ";// Add the 1st DNS Server Address.
  $networkCmnd .= $peNetCon->dnsServer2;// Add the 2nd DNS Server Address.
  exec($networkCmnd, $result, $retVal);// Execute the networksetup cmnd.
  if($retVal == 1) {                  // Does the return value indicate failure?
    return "The computer failed to accept " . $networkCmnd . "!";// Yes. Create an error message.
  } else {                            // The networksetup cmnd succeeded.
    return "";                        // Don't return an error message.
  } // End of condition checking the result of executing the networksetup.
} // End of function peSetNetwork.

function getBeds() {                  // Get all the beds and their chnls.
  $theBeds = array();                 // Create an array to hold each bed's configuration.
  $beds = Bed::all();                 // Access the DB beds.
  $theBeds['noBeds'] = sizeof($beds); // Contains the number of beds.
  $bDex = 0;                          // Contains the bed idex.
  foreach ($beds as $aBed) {          // Go through each bed.
    // Get all the channels on this bed.
    $bedChnls = DB::connection('sqlite')->table('bedChnls')->where('bedId', $aBed['id'])->get();
    $bed = array();                   // Create an array to contain the bed's configuration.
    $bed['name'] = $aBed->name;       // Save the bed's name.
    $bed['noChnls'] = sizeof($bedChnls);// Contains the number of channels of this bed.
    $cDex = 0;                        // Contains the bed channel index.
    foreach ($bedChnls as $aChnl) {   // Go throung each channel on the bed.
      $chnlId = "bedChnl" . $cDex++;  // Create a label for the bed channel.
      $aBedChnl = array();            // Create an array to hold the configuration of a channel on the bed.
      $aBedChnl['scChnlDex'] = $aChnl->scChnlDex;// Save the scChnl index.
      $aBedChnl['bedChnlRefDex'] = $aChnl->bedChnlRefDex;// Save the reference's scChnl index.
      $aBedChnl['controlType'] = $aChnl->controlType;// Save how this channel is controlled.
      $aBedChnl['assignedChnlDex'] = $aChnl->assignedChnlDex;// Save the scChnlDex of the assigned chnl.
      $bed[$chnlId] = $aBedChnl;      // Add the channel to the bed.
    }                                 // All this bed's channels are configured.
    $theBeds['bed' . $bDex++] = $bed; // Add this bed to the array of beds.
  }                                   // All the beds have been configured.
  return $theBeds;                    // Pass the bed configurations back to the caller.
} // End of function getBeds().

function saveChnls($chnls) {         // Save all the chnl configurations.
  // Save the changes to the configuration of any channels.
  $chnlTable = DB::connection('sqlite')->table('chnls');// Connect to the database's chnls table.
  deleteAllChnls();                   // Delete all the chnls in the DB.
  for($chnlDex = 0; $chnlDex < sizeof($chnls); $chnlDex++) {// Go through each channel.
    $thisChnl = $chnls[$chnlDex];     // Get the cofiguration just set for this chnl.
    $chnlTable->insert(               // Add this chnl to the
      [                               // database chnls table.
        'id' => $thisChnl['id'],      // Set the chnl's id.
        'name' => $thisChnl['name'],  // Set its name.
        'deviation' => $thisChnl['deviation'],// Set its deviation.
        'approachLimit' => $thisChnl['approachLimit'],// Set its approach limit.
        'inputNo' => $thisChnl['inputNo'],// Set its input number.
        'relayNo' => $thisChnl['relayNo'],// Set its relay number.
        // !!!!
        //'plotTemperature' => $thisChnl['plotTemperature'],// Set the plot this chnl's temperature flag.
        //'colorHue' => $chnls[$chnlDex]['colorHue'],// Set the plot color hue.
        //'colorSaturation' => $chnls[$chnlDex]['colorSaturation'],// Set the plot color saturation.
        //'colorLightness' => $chnls[$chnlDex]['colorLightness'],// Set the plot color lightness.
        //'plotProgram' => $thisChnl['plotProgram'],// Set the plot this chnl's program flag.
        //'prgHue' => $chnls[$chnlDex]['prgHue'],// Set the program color hue.
        //'prgSaturation' => $chnls[$chnlDex]['prgSaturation'],// Set the program color saturation.
        //'prgLightness' => $chnls[$chnlDex]['prgLightness'] // Set the program color lightness.
        // !!!!
      ] // This is all the data that makes up a chnl's configuration.
    ); // End of the chnl configuration record.
  } // End of looping through all the chnls to save their configurations to the DB.
} // End of function saveChnls.

function deleteAllChnls() {
  $chnls = Chnl::all();               // Access the DB chnls.
  foreach($chnls as $aChnl ) {        // Go through each chnl and delete it.
    DB::connection('sqlite')->table('chnls')->where('id', $aChnl['id'])->delete();// Delete this chnl.
  } // End of looping through all the chnls to delete them.
} // End of function deleteAllChnls.

function saveBeds($theBeds) {         // Save all the beds and their chnls.
  $noBeds = $theBeds["noBeds"];       // Get the number of beds to save.
  $bedTable = DB::connection('sqlite')->table('beds');// Connect to the database's beds table.
  $bedChnlsTable = DB::connection('sqlite')->table('bedChnls');// Connect to the database's bed chnls table.
  $chnlId = 1;                        // Holds the unique id for each bed chnl in the bed chnls table.
  deleteAllBeds();                    // Delete all the beds and their chnls in the DB.
  for ($bedDex = 0; $bedDex < $noBeds; $bedDex++) {// Go through each bed in $theBeds.
    $bedTxt = "bed" . $bedDex;        // Create the key for the bed.
    $aBed = $theBeds[$bedTxt];        // Get this bed.
    $bedTable->insert(                // Add this bed to the
      [                               // database beds table.
        'id' => $bedDex + 1,          // First set the bed's id.
        'name' => $aBed['name'],      // Then set its name.
      ]                               // This is all the data that makes up the
    );                                // bed table.
    for ($bedChnlDex = 0; $bedChnlDex < $aBed['noChnls'] ; $bedChnlDex++) {// Go through each chnl on this bed.
      $bedChnlTxt = "bedChnl" . $bedChnlDex;// Create the key for this bed chnl.
      $aChnl = $aBed[$bedChnlTxt];    // Get this bed chnl.
      $bedChnlsTable->insert(         // Add this bed chnl to the
        [                             // database's bed chnl table.
          'id' => $chnlId++,          // First set the bed chnl's id.
          'scChnlDex' => $aChnl['scChnlDex'], // Next set the scChnl index which this bed chnl is associated.
          'bedChnlRefDex' => $aChnl['bedChnlRefDex'], // Next set the reference scChnl index which this bed chnl is associated.
          'controlType' => $aChnl['controlType'],// Next set how this bed chnl is controlled.
          'assignedChnlDex' => $aChnl['assignedChnlDex'],// Next set the scChnlDex of the assigned chnl (if there is one).
          'bedId' => $bedDex + 1      // Reference the ones based bed id which contains this bed chnl.
        ]                             // This is all the data that makes up the
      );                              // bed table.
    } // End of loop through the bedChnls.
  } // End of loop through the beds.
} // End of function saveBeds().

function deleteAllBeds() {            // Delete all the beds and their chnls.
  $beds = Bed::all();                 // Access the DB beds.
  foreach($beds as $aBed ) {          // Go through each bed.
                                      // Delete all the channels on this bed.
    DB::connection('sqlite')->table('bedChnls')->where('bedId', $aBed['id'])->delete();
                                      // Delete the bed.
    DB::connection('sqlite')->table('beds')->where('id', $aBed['id'])->delete();
  }                                   // All the bed chnls and beds have been deleted.
} // End of function deleteAllBeds().

function alertPM($command, $numbers) {// Alert the PM by setting values in the pm_alerts table.
  $error = "";                        // There is no error yet.
  $pmAlert = \App\PmAlert::find(1);   // Get the PM Alerts table.
  $pmAlert->command = $command;       // Tell the PM what to do.
  $pmAlert->number = $numbers;        // This is the number associated with the command.
  $pmAlert->save();                   // Write the command to the PM Alerts table.
  $attemptMax = 10; // Wait for up to 10 seconds for the plant manager to reply that it read the command.
  if($command == "Stop") {            // Is the Stop command being issued?
    $attemptMax = 60;                 // Yes. The Stop command requires extra time.
  } // End of condition to check for the stop command.
  for($attempt = 0; $attempt < $attemptMax; $attempt++) {// Attempt multiple times to get a reply from the PM.
    $pmReply = \App\PmReply:: find(1);// Get the PM reply table.
    if ( $pmReply->reply == "PM_SUCCESS") {// Was the reply PM_SUCCESS?
      break;                          // Yes. There is no need to wait longer for the reply.
    }                                 // The reply has not been received yet.
    sleep(1);                         // Wait for one second.
  } // End of loop which waits for a reply for the PM.
  $pmAlert->command = "PM_NONE";      // Reset the alert table to remove the command.
  $pmAlert->save();                   // Write the PM_NONE command to the alert table. The PM will know things are done.
  if($attempt >= $attemptMax) {       // There was a time out before the reply was received.
    // Create an error message.
    $error = "The Plant Manager didn't read the command " . $command . ".";
  }                                   // Done.
  // Wait for up to 10 seconds for the plant manager to reply that it read the command.
  for($attempt = 0; $attempt < $attemptMax; $attempt++) {// Attempt up to ten times to get a reply from the PM.
    $pmReply = \App\PmReply:: find(1);// Get the PM reply table.
    if ( $pmReply->reply == "PM_NONE") {// Was the reply PM_NONE?
      break;                          // Yes. There is no need to wait longer for the reply.
    }                                 // The reply has not been received yet.
    sleep(1);                         // Wait for one second.
  }                                   // The PM has replied with PM_SUCCESS and cleared the reply table with PM_NONE.
  if($attempt >= $attemptMax) {                // Did the PM fail to reset the reply table?
    $error =  "The Plant Manager never reset its reply table to PM_NONE!";// Yes. Create an error message.
    $pmReply->reply = "PM_NONE";      // Reset the PM's reply to PM_NONE.
    $pmReply->save();                 // Put the PM_NONE reply in the PM's reply table.
  }                                   // Done communicating with the PM.
  return $error;                      // Let the caller know if there were any errors.
}

function plantManagerConnect($scErrors) {
  // Create the socket to communicate with the Plant Manager.
/*
  $client = stream_socket_client("tcp://10.0.0.98:64900", $errno, $errorMessage);
  if($client === false){
    echo "it failed <br>";
    //return FALSE;
    throw new UnexpectedValueException("Failed to connect: $errorMessage");
  }
  echo "it worked <br>";
  return FALSE;
*/
  if(!($sock = socket_create(AF_INET, SOCK_STREAM, 0))) // Was the socket created?
  {                                                     // No.
      $errorcode = socket_last_error();                 // Get the error code.
      $errormsg = socket_strerror($errorcode);          // Turn the error code into a message.
      $errormsg = "Socket error: " . $errormsg;         // Create an error message for a socket with the error description.
      array_push($scErrors, $errormsg);                  // Put the error in the error array.
      return FALSE;                                     // Return failure.
  }

  // Connect to the Plant Manager using the previously created socket. The port is always 64900.
//  if(socket_connect($sock , "10.0.0.98" , 64900) == FALSE) {
  if(socket_connect($sock , "10.0.0.98" , 64900) == FALSE) {
    $errorcode = socket_last_error();                 // Get the error code.
    $errormsg = socket_strerror($errorcode);          // Turn the error code into a message.
    $errormsg = "Socket error: " . $errormsg;         // Create an error message for a socket with the error description.
    array_push($scErrors, $errormsg);                  // Put the error in the error array.
    return FALSE;                                     // Return failure to connect.
  }
  return $sock;                                       // Return the socket used to connect to the Plant Manager.
}

function plantManagerSend($command) {
  $allData = array();                     // The 1st element contains the error message. The 2nd contains the data array.
  $data = array();                        // Create an array to hold the data.
  // Create the socket to communicate with the Plant Manager.
  // Possible future, fix plantManagerConnect, it will not return an error message.
  if(!($sock = plantManagerConnect($allData))) {  // Was the connection to the Plant Manager established?
    array_push($allData, "Error: Could not connect to the Plant Manager!");
    return $allData;                       // No. There was an error or errors.
  }                                         // The Plant Manager connection was established.
  // Send the Plant Manager a command.
  if(!(socket_send ( $sock , $command , strlen($command) , 0))) { // Was the command sent?
    $errorcode = socket_last_error();       // No. Get the error code.
    $errormsg = socket_strerror($errorcode);// Turn the error code into a message.
    $errormsg = "Error: " . $errormsg;      // Create an error message for a socket with the error description.
    array_push($allData, $errormsg);       // Put the error in the error array.
    return $allData;                       // Return the error.
  } // The command was successfully sent to the Plant Manager.
  // Now receive the reply from the Plant Manager.
  if(socket_recv ( $sock , $buf , 8096 , MSG_PEEK ) === FALSE) { // Was the relay data received?
    $errorcode = socket_last_error();       // No. Get the error code.
    $errormsg = socket_strerror($errorcode);// Turn the error code into a message.
    $errormsg = "Error: " . $errormsg;      // Create an error message for a socket with the error description.
    array_push($allData, $errormsg);       // Put the error in the error array.
    return $allData;                       // Return the error.
  } // The reply data has been received from the Plant Manager.
  socket_close($sock);                      // Close the connection to the Plant Manager.
  $data = preg_split("/\n/", $buf);         // Split the return relay data by 'new line' characters.
  array_push($allData, "No errors!");
  array_push($allData, $data);
  return $allData;                    // Send the reply to the caller.
}

function buildRHData($relays, $peLS) {
  $rhData = array();                  // Create an array to hold all the time/temperature data for all channels being plotted.
  $theTimeZone = \App\Configuration::find(1)->timeZone;// Get the time zone.
  $pmSettings = \App\PMSetting::find(1);// Open the pmSettings table to get the data storage interval.
  $storeInt = $pmSettings->storageInterval;// Save the storage interval.
  $startStr = $peLS['PE_Relay_StartTime'];// Get the time when plotting should begin.
  $timeSpan = $peLS['PE_Relay_Span']; // Get the number of hours to be plotted.
  $dt = new DateTime($startStr, new DateTimeZone($theTimeZone));// Create a DateTime for the time zone.
  $dt->setTimeZone(new DateTimeZone("UTC"));// Change the time zone (and time) to UTC.
  $utcStartStr = $dt->format("Y-m-d H:i");// Create the UTC start time accurate to the minute.
  $timeCarbon = new Carbon($startStr);// Convert the time to a carbon time for calculating the end time.
  $timeCarbon->addHours($timeSpan);   // Add the plot span in hours to the start time to get the end time.
  $endStr = $timeCarbon->format('Y-m-d H:i');// Convert the end time into a format to query the relay history database.
  $timeCarbon = new Carbon($utcStartStr);// Convert the time to a carbon time for calculating the end time.
  $timeCarbon->addHours($timeSpan);   // Add the plot span in hours to the start time to get the end time.
  $utcEndStr = $timeCarbon->format('Y-m-d H:i');// Convert the end time into a format to query the relay history database.
  foreach ($relays as $aRelay) {      // Go through each relay in the system.
    $tableNo = $aRelay['id'] - 1;     // Yes. Each relay's time-ons are stored in a table that contains a number.
    if($peLS['PE_Relay_Plot'][$tableNo] == 1) { // Is this relay supposed to be plotted?
      $tableName = "tim" . $tableNo;  // Put "tim" in front of the number to get this relay's history table name.
      // Query this relay's time-on table for all the time-ons between the start and finish times.
      // The results are unpredictable. If the PM recorded to the db during the whole time interval, then there
      // will be a smooth graph. However, relay history points could be missing anywhere in the interval. In fact,
      // there could be no points in the interval. Both the real and missing relay history points are processed
      // in the code which follows.
      $times = DB::connection('sqlite3')->table($tableName)->whereBetween('sc_time', [$utcStartStr, $utcEndStr])->get();
      if(sizeof($times) == 0) {       // Were there any times found?
        $timData = array(             // No. Create an array to
          array(                      // hold this relay's time-on data.
            // The "x" and "y" tags are required by jsChart. "x" contains the time and "y" contains the percentage of time on.
            "x" => $startStr,         // Set the start time in the array's first element. Format('Y-m-d H:i').
            "y" => -1000              // Set the start time-on far below zero to hide the graph.
          ),                          // This is the end of the first element.
          array(                      // Create the second element in the array to hold the finish time & time-on.
            "x" => $endStr,           // Set the time to the finish time.
            "y" => -10000             // Set the time-on to below zero to prevent a graph.
          )                           // The array is complete for a chnl which has no temperatures.
        );  // The array consists of arrays which contain a time "x" and a percentage "y".
        array_push($rhData, $timData);// Append this relay's time/percentage on array to the other relays arrays.
        continue;                     // Go to examine the next relay.
      } // End of conditon: No time-on points were found in the database.
      $startTime = $startStr;         // Set the start time to the user's desired start time.
      $timeOn = $times[0]->secondsOn; // Set the start time-on to the first time-on in the requested
                                      // range. Because the first retrieved point may not have the same time
                                      // as the start time, the first retrieved time must be checked to see
      if($utcStartStr != $times[0]->sc_time) { // if it matches the start time. Do the two times match?
        $timeOn = -10000;             // No. The times didn't match. Set the time-on down scale.
      } // End of condition: The start times don't match.
      // Convert the time-on in seconds to the percent of the storage interval the relay was on.
      $percentOn =  $timeOn / $storeInt / 60 * 100;
      // The starting percentage of the storage interval on is correctly set at this point.
      $timData = array(               // Create an array to hold this
        array(                        // relay's percetage of time on during the storage interval.
          // The "x" and "y" tags are required by jsChart. "x" contains the time and "y" contains the temperature.
          "x" => $startTime,          // Set the start time in the array first element. Format('Y-m-d H:i').
          "y" => $percentOn           // Set the start percentage.
        ),                            // This is the end of the first element.
      );  // The array consists of arrays which contain a time "x" and a percentage "y".
      $timeGaps = 0;                  // Contains the number of artificial points which had to be added
                                      // for when recordings were missed in the database.
      $utcTime = Carbon::createFromFormat('Y-m-d H:i', $utcStartStr);;// $utcTime is the next calculated utc plot time.
      if($utcStartStr != $times[0]->sc_time) { // If the desired start time doesn't match the recorded start time,
        // a recording was missed. A line will be drawn from desired start time to the first recorded time after
        // the desired start time which is downscale. Then, a line will be drawn from downscale at the first recorded
        // time after the desired start time up to the recorder time-on at that first recorded time.
        $dt = new DateTime($times[0]->sc_time, new DateTimeZone("UTC"));// Create a time for the first recorded time-on.
        $dt->setTimeZone(new DateTimeZone($theTimeZone));// Convert the time to the users time zone.
        $localTimeStr = $dt->format("Y-m-d H:i");// Create a Chartjs compatible time.
        $timData[1] = array(          // Create an artificial time/percetage on point
          'x' => $localTimeStr,       // with a time which is equal to the first recorded time after the desired
          'y' => -10000               // start time and a percentage on which is downscale.
        );                            // The artificial point has been created.
        $timeOn = $times[0]->secondsOn;// Next, get the first recorded time-on after the desired start time.
        // Convert the time-on in seconds to the percentage of the storage interval the relay was on.
        $percentOn =  $timeOn / $storeInt / 60 * 100;
        $timData[2] = array(          // Next, create an artificial
          'x' => $localTimeStr,       // time/percentage on point to drive the graph
          'y' => $percentOn           // up to the actual first recorded time-on.
        );                            // The artificial point has been created.
        $timeGaps = 2;                // Two artificial points were used.
        $utcTime = Carbon::createFromFormat('Y-m-d H:i', $times[0]->sc_time);// Put the time from the db in carbon format.
      } // End of condition: The recorded start time didn't match the desired start time.
      $len = count($times);           // Contains the number of recordings returned from the db for this relay.
      for($i = 1; $i < $len; $i++) {  // Go through all of this relay's recordings.
        $utcTime->addMinutes($storeInt);// Add interval between samples to get the next utc time which should be plotted.
        $dt = new DateTime($utcTime, new DateTimeZone("UTC"));// Create a new DateTime in the UTC time zone.
        $dt->setTimeZone(new DateTimeZone($theTimeZone));// Now change the time zone to the one set in the configurations
                                      // table of the SCConfig.db database. (the users time zone)
        $localTimeStr = $dt->format("Y-m-d H:i");// Chartjs requires the plot time format to use the users time zone.
        $rcrdTime = Carbon::createFromFormat('Y-m-d H:i', $times[$i]->sc_time);// Put the time from the db in carbon format.
        if($utcTime->notEqualTo($rcrdTime)) {// Are the db time and calculated plot time different?
          $timData[$i + $timeGaps] = array(// Yes. They are different. A recording was missed. Create an artificial
            'x' => $localTimeStr,     // time/percentage on point
            'y' => -10000             // to drive the graph down scale.
          );                          // The artificial point has been created.
          $timeGaps++;                // Increment the number of artificial points which have been added.
          $dt = new DateTime($rcrdTime, new DateTimeZone("UTC"));// The next point displayed will be at the time which
                                      // was recorded. This time is recorded using the UTC time zone.
          $dt->setTimeZone(new DateTimeZone($theTimeZone));// Change the time to the users time zone.
          $localTimeStr = $dt->format("Y-m-d H:i");// Put the time in the format required by Chartjs.
          $timData[$i + $timeGaps] = array(// Create another artificial percentage on point which is down scale but
                                      // at the time of the recorded time-on.
            'x' => $localTimeStr,     // Put this in the array that will be passed to Chartjs.
            'y' => -10000             // The 'y' coordinate is the percentage on which is down scale.
          );                          // The second artificial point has been created.
          $timeGaps++;                // Increment the number of artificial points which have been added. ( a total of two)
        } // End of condition: The db's recorded time and calculated plot time are different.
        $timeOn = $times[$i]->secondsOn;  // Get the next time-on.
        // Convert the time-on in seconds to the percentage of the storage interval the relay was on.
        $percentOn =  $timeOn / $storeInt / 60 * 100;
        $timData[$i + $timeGaps] = array(// Add the next
          'x' => $localTimeStr,       // time
          'y' => $percentOn           // percentage on point to the array.
        );                            // This real time/percetage on point has been added.
        $utcTime = $rcrdTime;         // Set $utcTime to the recorded time in order to check the next recorded time.
      } // Done looping through all of this relay's recordings.
      // Append the array of percentage on data for this relay to the array containing all the other plot relays.
      array_push($rhData, $timData);
    } // End of condition: This relay should be plotted.
  } // End of looping through each relay.
  return $rhData;                     // Return the time/percetage on data for all relays to be plotted.
} // End of functionbuildRHData.

function buildTvtData($chnls, $peLS) {
  $tvtData = array();         // Create an array to hold all the time/temperature data for all channels being plotted.
  $theTimeZone = \App\Configuration::find(1)->timeZone;// Get the time zone.
  $pmSettings = \App\PMSetting::find(1);// Open the pmSettings table to get the data storage interval.
  $storeInt = $pmSettings->storageInterval;// Save the storage interval.
  $dt = new DateTime($peLS['PE_TVT_StartTime'], new DateTimeZone($theTimeZone));// Create a DateTime for the time zone.
  $dt->setTimeZone(new DateTimeZone("UTC"));// Change the time zone (and time) to UTC.
  $utcStartStr = $dt->format("Y-m-d H:i");// Create the UTC start time accurate to the minute.
  $timeCarbon = new Carbon($peLS['PE_TVT_StartTime']);// Convert the time to a carbon time for calculating the end time.
  $timeCarbon->addHours($peLS['PE_TVT_Span']);// Add the plot span in hours to the start time to get the end time.
  $endStr = $timeCarbon->format('Y-m-d H:i');// Convert the end time into a format to query the tvt database.
  $timeCarbon = new Carbon($utcStartStr); // Convert the time to a carbon time for calculating the end time.
  $timeCarbon->addHours($peLS['PE_TVT_Span']);// Add the plot span in hours to the start time to get the end time.
  $utcEndStr = $timeCarbon->format('Y-m-d H:i');// Convert the end time into a format to query the tvt database.
  foreach ($chnls as $aChnl) {            // Go through each channel in the system.
    if($peLS['PE_Chnl_Plot'][$aChnl['id'] - 1] == 1) {// Is this channel supposed to be plotted?
      $tableNo = $aChnl['id'] - 1;        // Yes. Each chnl's temperatures are stored in a table that contains a number.
                                          // The number is one less than the chnl's id (table number is zero based).
      $tableName = "tvt" . $tableNo;      // Put "tvt" in front of the number to get this chnl's temperature table name.
      // Query this chnl's temperature table for all the temperatures between the start and finish times.
      // The results are unpredictable. If the PM recorded to the db during the whole time interval, then there
      // will be a smooth graph. However, time/temperature points could be missing anywhere in the interval. In fact,
      // there could be no points in the interval. Both the real and missing time/temperature points are processed
      // in the code which follows.
      $temps = DB::connection('sqlite2')->table($tableName)->whereBetween('sc_time', [$utcStartStr, $utcEndStr])->get();
      if(sizeof($temps) == 0) {           // Were there any temperatures found?
        $tvtChnlData = array(             // No. Create an array to
          array(                          // hold this chnl's time/temp data.
            // The "x" and "y" tags are required by jsChart. "x" contains the time and "y" contains the temperature.
            "x" => $peLS['PE_TVT_StartTime'],// Set the start time in the array's first element. Format('Y-m-d H:i').
            "y" => -10000                 // Set the start temperature far below any reasonable temperature to hide the graph.
          ),                              // This is the end of the first element.
          array(                          // Create the second element in the array to hold the finish time & temperature.
            "x" => $endStr,               // Set the time to the finish time.
            "y" => -10000                 // Set the temperature very low to prevent a graph.
          )                               // The array is complete for a chnl which has no temperatures.
        );  // The array consists of arrays which contain a time "x" and a temperature "y".
        array_push($tvtData, $tvtChnlData);// Append this chnl's time/temp array to the other chnls time/temp arrays.
        continue;                         // Go to examine the next channel.
      } // End of conditon: No time/temp points were found in the database.
      $startTime = $peLS['PE_TVT_StartTime'];// Set the start time to the user's desired start time.
      $startTemp = $temps[0]->temperature;// Set the start temperature to the first temperature in the requested
                                          // range. Because the first retrieved point may not have the same time
                                          // as the start time, one must check the first retrieved time to see
      if($utcStartStr != $temps[0]->sc_time) { // if it matches the start time. Do the two time match?
        $startTemp = -10000;              // No. The times didn't match. Set the temperature down scale.
      }                                   // The start temperature is correctly set.
      if($peLS['PE_TVT_Units'] == "C") {     // Is the user plotting in Celsius?
        $startTemp = ($startTemp - 32) / 1.8;// Yes. Convert the temperature to Celsius.
      }                                   // The temperature has the correct units.
      $tvtChnlData = array(               // Create an array to
        array(                            // hold this chnl's time/temp data.
          // The "x" and "y" tags are required by jsChart. "x" contains the time and "y" contains the temperature.
          "x" => $startTime,              // Set the start time in the array first element. Format('Y-m-d H:i').
          "y" => $startTemp               // Set the start temperature.
        ),                                // This is the end of the first element.
      );  // The array consists of arrays which contain a time "x" and a temperature "y".
      $timeGaps = 0;                      // Contains the number of artificial points which had to be added
                                          // for when recordings were missed in the database.
      $utcTime = Carbon::createFromFormat('Y-m-d H:i', $utcStartStr);;// $utcTime is the next calculated utc plot time.
      if($utcStartStr != $temps[0]->sc_time) { // If the desired start time doesn't match the recorded start time,
        // a recording was missed. A line will be drawn from desired start time to the first recorded time after
        // the desired start time which is downscale. Then, a line will be drawn from downscale at the first recorded
        // time after the desired start time up to the recorder temperature at that first recorded time.
        $dt = new DateTime($temps[0]->sc_time, new DateTimeZone("UTC"));// Create a time for the first recorded temperature.
        $dt->setTimeZone(new DateTimeZone($theTimeZone));// Convert the time to the users time zone.
        $localTimeStr = $dt->format("Y-m-d H:i");// Create a Chartjs compatible time.
        $tvtChnlData[1] = array(          // Create an artificial time/temperature point
          'x' => $localTimeStr,           // with a time which is equal to the first recorded time after the desired
          'y' => -10000                   // start time and a temperature which is downscale.
        );                                // The artificial point has been created.
        $theTemp = $temps[0]->temperature;// Next, get the first recorded temperature after the desired start time.
        if($peLS['PE_TVT_Units'] == "C") {     // Is the user plotting in Celsius?
          $theTemp = ($theTemp - 32) / 1.8;// Yes. Convert the temperature to Celsius.
        }                                 // The temperature has the correct units.
        $tvtChnlData[2] = array(          // Next, create an artificial
          'x' => $localTimeStr,           // time/temperature point to drive the graph
          'y' => $theTemp                 // up to the actual first recorded temperature.
        );                                // The artificial point has been created.
        $timeGaps = 2;                    // Two artificial points were used.
        $utcTime = Carbon::createFromFormat('Y-m-d H:i', $temps[0]->sc_time);// Put the time from the db in carbon format.
      }                                   // We are now ready to go through all the recorded points.
      $len = count($temps);               // Contains the number of temperatures returned from the db for this chnl.
      for($i = 1; $i < $len; $i++) {      // Go through all of this chnl's time/temperature data.
        $utcTime->addMinutes($storeInt);  // Add interval between samples to get the next utc time which should be plotted.
        $dt = new DateTime($utcTime, new DateTimeZone("UTC"));// Create a new DateTime in the UTC time zone.
        $dt->setTimeZone(new DateTimeZone($theTimeZone));// Now change the time zone to the one set in the configurations
                                          // table of the SCConfig.db database. (the users time zone)
        $localTimeStr = $dt->format("Y-m-d H:i");// Chartjs requires the plot time format to use the users time zone.
        $rcrdTime = Carbon::createFromFormat('Y-m-d H:i', $temps[$i]->sc_time);// Put the time from the db in carbon format.
        if($utcTime->notEqualTo($rcrdTime)) {// Are the db time and calculated plot time different?
          $tvtChnlData[$i + $timeGaps] = array(// They are different. A recording was missed. Create an artificial
            'x' => $localTimeStr,         // time/temperature point
            'y' => -10000                 // to drive the graph down scale.
          );                              // The artificial point has been created.
          $timeGaps++;                    // Increment the number of artificial points which have been added.
          $dt = new DateTime($rcrdTime, new DateTimeZone("UTC"));// The next point displayed will be at the time which
                                          // was recorded. This time is recorded using the UTC time zone.
          $dt->setTimeZone(new DateTimeZone($theTimeZone));// Change the time to the users time zone.
          $localTimeStr = $dt->format("Y-m-d H:i");// Put the time in the format required by Chartjs.
          $tvtChnlData[$i + $timeGaps] = array(// Create another artificial time/temp point which is down scale but
                                          // at the time of the recorded time/temp point.
            'x' => $localTimeStr,         // Put this in the array that will be passed to Chartjs.
            'y' => -10000                 // The 'y' coordinate is the temeprature which is down scale.
          );                              // The second artificial point has been created.
          $timeGaps++;                    // Increment the number of artificial points which have been added. ( a total of two)
        }                                 // The artificial points were added if required.
        $theTemp = $temps[$i]->temperature;// Get the next temperature.
        if($peLS['PE_TVT_Units'] == "C") { // Is the user plotting in Celsius?
          $theTemp = ($theTemp - 32) / 1.8;// Yes. Convert the temperature to Celsius.
        }                                 // The temperature has the correct units.
        $tvtChnlData[$i + $timeGaps] = array(// Add the next
          'x' => $localTimeStr,           // time
          'y' => $theTemp                 // temperature point to the array.
        );                                // This real time/temperature point has been added.
        $utcTime = $rcrdTime;             // Set $utcTime to the recorded time in order to check the next recorded time.
      }                                   // All the time/temperature points have been added.
      // Append the array of time vs temperature data for this channel to the array containing all the other plot channels.
      array_push($tvtData, $tvtChnlData);
    } // End of condition: this channel needs to be plotted.
  } // End of looping through all the chnls and processes those being plotted.
  return $tvtData;                        // Return the time/temperature data for all chnls to be plotted.
} // End of function buildTvtData.

// Given the channel index, return the Chartjs
// array for the program so it can be plotted.
function buildPrgChart($prgDex) {
  // Get the given chnl's program points from the programPoints table and put them in Chartjs format.
  // $prgDex is zero based. But, the database is one based. Therefore, one must be added to $prgDex.
  $prgPoints = DB::connection('sqlite')->table('programPoints')->where('programId', $prgDex + 1)->get();
  $prgChart = array(                  // Create an array to
    array(                            // hold this program's time/temp points.
      // The "x" and "y" tags are required by Chartjs.
      // "x" contains the time in elapsed hours since the start time and "y" contains the temperature.
      'x' => 0,                       // The elapsed hours since the start time for the first point is always zero.
      'y' => $prgPoints[0]->temp      // Set the start temperature.
    ),                                // This is the end of the first element.
  );  // The array consists of arrays which contain elpased hours "x" and a temperature "y".
  $i = 1;                             // The index of the point being configured.
  if(sizeof($prgPoints) < 2) {        // A legal program has more than one point. Is this a legal program?
    extract(getDefaultDBProgram());   // No. Create a new default program.
    saveDBPrgPoints($prgDex, $thePrgPoints);// Save the program points.
    saveDBPrg($prgDex, $theDBPrg);    // Save the program.
  }
  while($prgPoints[$i]->time != 65000) { // Is there a flag to indicate the program is over?
    $prgChart[$i] = array(            // Add the next program point to the program chart.
      'x' => $prgPoints[$i]->time / 60,// Set the x coordinate for this program point (convert elpased minutes to elapsed hours).
      'y' => $prgPoints[$i]->temp     // Set the y coordinate for this program point (a temperature).
    );                                // This program point has been set to chart
    $i++;                             // Increment the index.
    if($i >= sizeof($prgPoints)) {    // Is there an accidental attempt to read past the end of the program?
      break;                          // Yes. Don't try to process any more points.
    }                                 // The index is valid for the next program point.
  }                                   // All the program points have been set to chart.
  return $prgChart;                   // Return the Chartjs format of the program points.
}

// Given the channel index, return the Chartjs array for this chnl's program
// containing real times so it can be plotted with time vs temeprature. This is almost
// the same as buildPrgChart except it uses real times instead of times relative to zero.
function buildTvtPrgChart($prgDex, $units) {
  // Get the given chnl's program points from the programPoints table and put them in Chartjs format.
  // $prgDex is zero based. But, the database is one based. Therefore, one must be added to $prgDex.
  // First get the program.
  $prgData = DB::connection('sqlite')->table('programs')->where('id', $prgDex + 1)->get();
  // Then get the points that make up the program.
  $prgPoints = DB::connection('sqlite')->table('programPoints')->where('programId', $prgDex + 1)->get();
  // Then get the users time zone.
  $theTimeZone = \App\Configuration::find(1)->timeZone;
  // Create a start time in the UTC time zone since that is how start times are stored.
  $dt = new DateTime($prgData[0]->timeToStart, new DateTimeZone("UTC"));
  // Convert the time to the user's time zone.
  $dt->setTimeZone(new DateTimeZone($theTimeZone));
  $startStr = $dt->format("Y-m-d H:i");// Create the string representation of the time for Chartjs.
  $carbonThisTime = new Carbon($startStr);// Convert the time to a carbon time for calculating the end time.
  $carbonStartTime = $carbonThisTime;
  $thePrgTemp = $prgPoints[0]->temp;// Get the program start temperature.
  if($units == "C") {               // Is the user plotting in Celsius?
    $thePrgTemp = ($thePrgTemp - 32) / 1.8;// Yes. Convert the temperature to Celsius.
  }                                 // The temperature has the correct units.
  $prgChart = array(                  // Create an array to
    array(                            // hold this program's time/temp points.
      // The "x" and "y" tags are required by Chartjs.
      // "x" contains the time and "y" contains the temperature.
      'x' => $startStr,               // The time when the program starts.
      'y' => $thePrgTemp              // Set the start temperature.
    ),                                // This is the end of the first element.
  );  // The array consists of arrays which contain times "x" and a temperatures "y".
  $i = 1;                             // The index of the point being configured.
  while($prgPoints[$i]->time != 65000) { // Is there a flag to indicate the program is over?
    // Add the program's elapsed minutes since the last program point to get the new program point time.
    $carbonThisTime->addMinutes($prgPoints[$i]->time - $prgPoints[$i - 1]->time);
    $tStr = $carbonThisTime->format('Y-m-d H:i');// Convert this time into a format that Chartjs can use.
    $thePrgTemp = $prgPoints[$i]->temp;// Get the program's next temperature.
    if($units == "C") {               // Is the user plotting in Celsius?
      $thePrgTemp = ($thePrgTemp - 32) / 1.8;// Yes. Convert the temperature to Celsius.
    }                                 // The temperature has the correct units.
    $prgChart[$i] = array(            // Add the next program point to the program chart.
      'x' => $tStr,                   // Set the x coordinate for this program point.
      'y' => $thePrgTemp              // Set the y coordinate for this program point (a temperature).
    );                                // This program point has been set to chart
    $i++;                             // Increment the index.
    if($i >= sizeof($prgPoints)) {    // Is there an accidental attempt to read past the end of the program?
      break;                          // Yes. Don't try to process any more points.
    }                                 // The index is valid for the next program point.
  }                                   // All the program points have been set to chart.
  return $prgChart;                   // Return the Chartjs format of the program points.
}

// Saves the program points given in $thePrgPoints to the database referencing the program index $prgDex.
function saveDBPrgPoints($prgDex, $thePrgPoints) {
  $pointTable = DB::connection('sqlite')->table('programPoints'); // Connect to the database.
  $pointTable->where('programId', $prgDex + 1)->delete();         // Delete the old program.
  for($point = 0; $point < sizeof($thePrgPoints); $point++) {     // Go through each program point.
    $aPoint = $thePrgPoints[$point];                              // Get this program point.
    $pointTable->insert(                                          // Add this program point to the
      [                                 // database programPoints table.
        'programId' => $prgDex + 1,     // First set the program this point is used with.
        'time' => $aPoint['x'],         // Set this point's time.
        'temp' => $aPoint['y'],         // Set this point's temperature.
        'prNo' => 999                   // Set this point's priority number.
      ]                                 // this is all the data that makes up a time
    );                                  // temperature point.
  }                                     // All the program points have been set.
  $pointTable->insert(                  // Insert the stop point.
    [                                   //
      'programId' => $prgDex + 1,       // It is associated with this program.
      'time' => 65000,                  // 65000 indicates the program is over.
      'temp' => -500,                   // -500 also indicates the program is over.
      'prNo' => 999                     // We need a priority number to complete the point.
    ]                                   //
  );                                    // All the program points have been saved to the database.
}

// Saves the program given in $thePrgData to the database referencing the program index $thePrgDex.
function saveDBPrg($thePrgDex, $thePrgData) {
  $prgTable = DB::connection('sqlite')->table('programs'); // Connect to the programs database.
//  $prgTable->where('id', $prgDex + 1)->delete();         // Delete the old program.
  $prgTable->insert(                  // Add this program to the
    [                                 // database's programs table.
      'id' => $thePrgDex + 1,         // First set the program's id .
      'type' => $thePrgData->type,    // Set this program's type.
      'refChnlDex' => $thePrgData->refChnlDex, // Set this program's reference index.
      'cutoffTemp' => $thePrgData->cutoffTemp, // Set this program's cutoff temperature.
      'maintenanceTemp' => $thePrgData->maintenanceTemp, // Set this program's maintenace tempearture.
      'timeToStart' => $thePrgData->timeToStart, // Set this program's start time.
      'timeCreated' => $thePrgData->timeCreated, // Set this program's creation time.
      'active' => $thePrgData->active, // Set if this program is currently active.
      'version' => $thePrgData->version // Set this program's version.
    ]                                 // This is all the data that makes up a
  );                                  // program.
}

// Modifies the program in the database referenced by $thePrgDex.
function modifyDBPrg($thePrgDex, $thePrgPoints, $prgData) {
  // Convert the program times from elapsed hours to elapsed minutes to store in the database.
  for($dex = 1; $dex < sizeof($thePrgPoints); $dex++) {// Go through each program point.
    $thePrgPoints[$dex]['x'] *= 60;   // Convert elapsed hours to elapsed minutes for the database.
  }                                       // All point times have been converted to elapsed minutes.
  saveDBPrgPoints($thePrgDex, $thePrgPoints);// Save this program's points.
  $theDBPrg = \App\Program::find($thePrgDex + 1); // Get the database's current settings for this program.
  $theDBPrg->type = $prgData['type']; // Store the set program type.
  $theDBPrg->refChnlDex = $prgData['refChnlDex'];// Store the set reference channel.
  if($prgData['type'] == 1) {         // Is this program a parent?
    $theDBPrg->refChnlDex = 9999;     // 9999 indicates there is no reference. This is a parent.
  } // End of condition to clear a reference for a parent program.
  $theDBPrg->cutoffTemp = $prgData['cutoffTemp'];// Store the set cutoff temperature.
  $theDBPrg->maintenanceTemp = $prgData['maintenanceTemp'];// Store the set maintenance temperature.
  $theTimeZone = \App\Configuration::find(1)->timeZone;// Get the local time zone.
  $startTime = $prgData['timeToStart'];// Get the start time string.
  $dt = new DateTime($startTime, new DateTimeZone($theTimeZone));// Convert the start time string to a time.
  $dt->setTimeZone(new DateTimeZone("UTC"));// Change to the UTC time.
  $utcStartStr = $dt->format("Y-m-d H:i");// Create a UTC start time string for the DB.
  $theDBPrg->timeToStart = $utcStartStr;// Store the UTC start time.
  $theDBPrg->timeCreated = Carbon::now('UTC')->format('Y-m-d H:i');// The creation time is UTC now.
  $theDBPrg->active = $prgData['active'];// Store the program's active state.
  $theDBPrg->version = "1";           // Set the version.
  $theDBPrg->save();                  // Save the modified program to the database.
}

// Creates a default program for chnls without a program in the DB.
// The function then adds this program to the DB for each channel without a program.
function createDefaultDBPrograms($programs, $chnls) {
  extract(getDefaultDBProgram());     // Get the default DB program, it consists of $theDBPrg and $thePrgPoints.
  $startDex = sizeof($programs);      // Get the index of the first chnl not to have a program.
  //echo "<script type='text/javascript'>alert('$startDex');</script>";
  $finishDex = sizeof($chnls) - 1;    // Get the index of the last chnl not to have a program.
  //echo "<script type='text/javascript'>alert('$finishDex');</script>";
  // Create programs for each chnl which is missing a program.
  for($pDex = $startDex; $pDex <= $finishDex; $pDex++){// The database is ones based.
    //echo "<script type='text/javascript'>alert('$pDex');</script>";
    saveDBPrgPoints($pDex, $thePrgPoints);// Save the program points.
    saveDBPrg($pDex, $theDBPrg);      // Save the program.
  }                                   // All missing programs have been created.
}

// Creates the default program. It is a parent starting now running for 120 minutes at 70 but not active.
function getDefaultDBProgram() {
  $thePrgPoints = array(              // No. Create an array to
    array(                        // hold this program's time/temp points.
      // The "x" and "y" tags are required by jsChart.
      // "x" contains the time in elapsed hours since the start time and "y" contains the temperature.
      'x' => 0,                   // The elapsed hours since the start time for the first point is always zero.
      'y' => 70                   // Set the start temperature.
    ),                            // This is the end of the first element.
    array(
      'x' => 120,
      'y' => 70
    )
  );  // The array consists of arrays which contain elpased hours "x" and a temperature "y".
  $theDBPrg = new Program;        // Create a new program object.
  $theDBPrg->type = 1;            // The default program type is Parent.
  $theDBPrg->refChnlDex = 9999;      // There is no reference channel.
  $theDBPrg->cutoffTemp = 400;    // No cuttoff until it is set. 400 will never be reached.
  $theDBPrg->maintenanceTemp = 68;// Just keep the temperature at average room temperature.
  $theDBPrg->timeToStart = Carbon::now('UTC')->format('Y-m-d H:i');// The default start time is now.
  $theDBPrg->timeCreated = $theDBPrg->timeToStart;// the program is being created now.
  $theDBPrg->active = 0;          // A program is not active until it is started by the user.
  $theDBPrg->version = 1;         // The current version of the program on 2019-12-17 is one.

  return compact("theDBPrg", "thePrgPoints");
}

// Creates a program (points plus program data) for each chnl on a given bed.
// $theBeds => the current bed configurations, $bed_Dex => the index of the bed being programmed,
// $prgData => the prg data for the bed, $prgPoints => the bed's program points,
// &$scErrors => a reference to the errors array in case there is a problem.
// $action => 'save' means the bed chnl programs are being saved, and
//            'setPrg' means the bed chnl programs are being saved and the PM is being told to start the programs.
function createBedChnlPrograms($theBeds, $bed_Dex, $prgData, $prgPoints, &$scErrors, $action) {
  $lastPointDex = sizeof($prgPoints) - 1;// Get the index of this program's last time/temp point.
  $lastPoint = $prgPoints[$lastPointDex];// Access this program's last time/temp point.
  $lastHour = $lastPoint["x"];        // Get this program's last elapsed hour since the start time.
  $bKey = 'bed' . $bed_Dex;           // Create the key to access the bed being programmed.
  $noChnls = $theBeds[$bKey]["noChnls"];// Get the number of chnl on the bed.
  $programs = Program::all();         // Get the programs for all chnls so the cutoff and maintenance temps
                                      // won't be lost for bed chnls which are not the primary parent.
  for($bChnlDex = 0; $bChnlDex < $noChnls; $bChnlDex++) {// Go through each bed chnl.
    $bcKey = "bedChnl" . $bChnlDex;   // Create the key for this bed chnl object.
    $thePrgData = $prgData;           // Create a copy of the given program so the copy can be modified.
    $thePrgPoints = $prgPoints;       // Create a copy of the given program points so they can be modified.
    $bedChnl = $theBeds[$bKey][$bcKey];// Access this bed chnl object.
    $type = $bedChnl['controlType'];  // Get this chnl's control type.
    $scChnlDex = $bedChnl['scChnlDex'];// Get the scChnlDex for this bed chnl. The scChnlDex corresponds to the
                                      // the program's id. Each scChnl has a program.
    if($scChnlDex < 0) {              // Is this a valid scChnlDex?
      $scChnlDex = $bedChnl['assignedChnlDex'];// No. Get the scChnl assigned to this bed chnl.
      if($scChnlDex == -1) {          // Is the assigned chnl valid?
        $error = "The bed channel with index " . $bChnlDex . " has not been assigned. ";// No. create
        $error .= "The bed action " . $action . " cannot be completed.";// an error message.
        array_push($scErrors, $error);// Add the error to the error array.
        return;                       // Stop starting or saving the bed chnl programs.
      } // End of checking for a valid assigned chnl.
    } // End of getting an assigned chnl.
    if($bChnlDex != 0) {              // Is this a bed chnl which is not the primary parent?
      $thePrgData['cutoffTemp'] = $programs[$scChnlDex]['cutoffTemp'];
      $thePrgData['maintenanceTemp'] = $programs[$scChnlDex]['maintenanceTemp'];
    } // End of preserving the cutoff and maintenance temperatures for chnls which are not the primary parent.
    $thePrgData['id'] = $scChnlDex + 1;// Set the DB id (ones based) for this program.
    $thePrgData['type'] = $type;      // Set this bed chnl's configured program control type.
    if($type == 1) {                  // Is the bed chnl's control type a parent.
      $thePrgData['refChnlDex'] = "9999";// 9999 indicates there is no reference and a parent has no reference.
    } else {                         // The desired control type is either child, cutoff, or mimic.
      // Set the refChnlDex. That is, the scChnlDex of the chnl this program is referenced to.
      $bedChnlRefDex = $bedChnl['bedChnlRefDex'];// Get the bed chnl index this bed chnl is referenced to.
      $bedChnlRef = $theBeds[$bKey]["bedChnl" . $bedChnlRefDex];// Access the reference bed chnl object.
      $refChnlDex = $bedChnlRef['scChnlDex'];// Get the scChnlDex for this referenced bed chnl.
      if($refChnlDex < 0) {           // Is this a valid scChnlDex (does an assigned chnl need to be set)?
        $refChnlDex = $bedChnlRef['assignedChnlDex'];// No. Get the scChnl assigned to this reference chnl.
        if($refChnlDex == -1) {       // Is the assigned chnl valid?
          $error = "The bed channel with index " . $bChnlDex . " has not had its reference assigned. ";// No. create
          $error .= "The bed action " . $action . " cannot be completed.";// an error message.
          array_push($scErrors, $error);// Add the error to the error array.
          return;                     // Stop starting or saving the bed chnl programs.
        } // End of checking for a valid assigned chnl.
      } // End of setting assignedd chnls.
      $thePrgData['refChnlDex'] = $refChnlDex;// Set the reference's scChnlDex for this program.
      if($type == 2 || $type == 5) {  // Is the desired control type child or cylinder?
        if($type == 5) {              // Yes. Is the desired control type a cylinder?
          $lastHour += 6;             // Yes. A cylinder should run for 6 hours past its parent.
        }                             // The last hour has been adjusted for a cylinder.
        $thePrgData['type'] = "2";    // Set this program's desired control type to child (a cylinder is a child).
        $thePrgPoints = array(        // Set the program points array to
          array(                      // hold this child program's time/temp points.
            // "x" contains the time in elapsed hours since the start time and "y" contains the temperature.
            'x' => 0,                 // The elapsed hours since the start time for the first point is always zero.
            'y' => 0                  // Set the child's start temperature to a default of zero.
          ),                          // This is the end of the first element.
          array(                      // Set the child program's second point.
            'x' => $lastHour,         // The elapsed time is the same as the primary bed chnl's last time.
            'y' => 0                  // The program's second temperature is the same as its first.
          )                           // The entire default child program has been created.
        );  // The array consists of arrays which contains elpased hours "x" and temperatures "y".
      } // End of setting a program for a child or cylinder.
    } // Done setting a program for a chnl which is not a parent.
    modifyDBPrg($scChnlDex, $thePrgPoints, $thePrgData);// Save this bed chnls program to the database.
    //echo("<br>The action is " . $action);
    if($action == "setPrg") {         // Does the user want to start (or stop) this bed chnl's program?
      $error = "";                    // Yes. Clear any existing error messages.
      $error = alertPM("SetProgram", $scChnlDex);// Tell the Plant Manager to read the program.
      if($error != "") {              // Was there an error?
        $chnls = Chnl::all();         // Get the channel data from the SCConfig.db database.
        $error .= " The channel was " . $chnls[$scChnlDex]["name"] . ".";// Append the chnl name to the error message.
        array_push($scErrors, $error);  // Yes. Add any errors to the error array.
      } // End of appending the chnl name to the error.
    } // End of telling the PM to start (or stop) a program.
  } // End of going through each bed chnl.
} // End of function createBedChnlPrograms().

// Turns each chnl on the given bed off (sets active to false, saves the change to active, has the PM reread the program).
// $theBeds => the current bed configurations, $bed_Dex => the index of the bed being turned off,
// &$scErrors => a reference to the errors array in case there is a problem.
function deactivateBedChnlPrograms($theBeds, $bed_Dex, &$scErrors) {
  $bKey = 'bed' . $bed_Dex;           // Create the key to access the bed being programmed.
  $noChnls = $theBeds[$bKey]["noChnls"];// Get the number of chnls on the bed.
  for($bChnlDex = 0; $bChnlDex < $noChnls; $bChnlDex++) {// Go through each bed chnl.
    $bcKey = "bedChnl" . $bChnlDex;   // Create the key for this bed chnl object.
    $bedChnl = $theBeds[$bKey][$bcKey];// Access this bed chnl object.
    $scChnlDex = $bedChnl['scChnlDex'];// Get the scChnlDex for this bed chnl. The scChnlDex correlates to the
                                      // this bed chnl's program id. Each scChnl has a program.
    if($scChnlDex == -1) {            // Is this an assigned chnl?
      $scChnlDex = $bedChnl['assignedChnlDex'];// Yes. Get the assigned chnl.
      if($scChnlDex == -1) {          // Has the chnl ever been asigned?
        return;                       // No. There is nothing to turn off.
      }                               // The assigned chnl exists and has been set.
    } // End of getting an assigned chnl.
    $prgId = $scChnlDex + 1;          // The program's id is one greater than the scChnlDex.
    $theDBPrg = \App\Program::find($prgId); // Get the database's current settings for this program.
    $theDBPrg->active = 0;            // Make the bed chnl's program inactive.
    $theDBPrg->save();                // Save the modified program to the database.
    $error = "";                      // Contains any error the PM may generate.
    $error = alertPM("SetProgram", $scChnlDex);// Tell the Plant Manager to read the program.
    if($error != "") {                // Was there an error?
      $chnls = Chnl::all();           // Get the channel data from the SCConfig.db database.
      $error .= " The channel was " . $chnls[$scChnlDex]["name"] . ".";// Append which chnl caused the PM to have problems.
      array_push($scErrors, $error);  // Yes. Add any errors to the error array.
    } // End of talking to the PM for this chnl.
  } // End of going through all of this bed's chnls.
} // End of function deactivateBedChnlPrograms().


// Returns an error message if all the given program points are not valid.
function validatePrgPoints($prgPoints) {
  for($pDex = 1; $pDex < sizeof($prgPoints); $pDex++) { // Go through all possible program points.
    $thisPoint = $prgPoints[$pDex];     // Get the point being evaluated.
    $previousPoint = $prgPoints[$pDex - 1];// Get the point prior to this point.
    if($thisPoint['x'] == -1) {         // Is the time invalid?
      $errorMsg = "The entered time for point " . $pDex . " is not valid!";
      return $errorMsg;                 // This is not allowed. This is an invalid point.
    }                                   // The temperature is at least a valid time.
    if($thisPoint['x'] <= $previousPoint['x']) { // Is this point's time less than the previous point's time.
      // Yes. Each point's time must be greater than the previous points time.
      $errorMsg = "The point " . $pDex . " time is less than the previous time!";
      return $errorMsg;                 // This is not allowed. This is an invalid point.
    }                                   // This point's time was valid.
    if($thisPoint['y'] <= -100) {       // Does this temperature have an error code?
      switch ($thisPoint['y']) {        // Yes. Create an error message for the error code.
        case '-100':                    // The entered temperature was not a number.
          $errorMsg = "The temperature entered for point " . $pDex . " was not a number!";
          break;                        // Message created.
        case '-101':                    // The entered temperature was too cold.
          $errorMsg = "The temperature entered for point " . $pDex . " was less than -99!";
          break;                        // Message created.
        case '-102':                    // The entered ttemeprature was too hot.
          $errorMsg = "The temperature entered for point " . $pDex . " was greater than 300!";
          break;                        // Message created.
        default:                        // This is not expected.
          $errorMsg = "The temperature entered for point " . $pDex . " was invalid!";
          break;                        // Message created.
      }                                 // The error message has been created for a temperature error flag.
      return $errorMsg;                 // Provide the error message to the caller.
    }                                   // There was no temperature error flag.
  }                                     // All the points have been evaluated.
  return "";                            // The program points are valid.
}

// Converts a UTC time string to a local time string.
function UTCToLocalTimeStr($utcTime) {
  $theTimeZone = \App\Configuration::find(1)->timeZone;// Get the local time zone.
  $dt = new DateTime($utcTime, new DateTimeZone("UTC"));// Create a UTC time.
  $dt->setTimeZone(new DateTimeZone($theTimeZone));// Get the local time of the UTC time.
  $localTimeStr = $dt->format("Y-m-d H:i");// Create a local time string.
  return $localTimeStr;               // Return the local time string to the caller.
}

function getPrimaryBedChnlDex($bedDex) {// Returns the scChnlDex of the primary (normally 1st) bed chnl.
  $theBeds = getBeds();               // Get an array containing each bed's configuration.
  if($theBeds['noBeds'] == 0) {       // Are there any beds?
    return -1;                        // No. There are no bed chnls. Return error.
  }                                   // There are bed chnls.
  $bKey = "bed" . strval($bedDex);    // Set the bed object key to the first bed.
  $bcKey = "bedChnl0";                // Set the chnl object key to the first bed chnl.
  $scChnlDex = $theBeds[$bKey][$bcKey]['scChnlDex'];// Get the scChnlDex.
  if($scChnlDex >= 0) {               // Is there a defined scChnlDex?
    return $scChnlDex;                // Yes. This is the the primary chnl's bed index.
  } else {                            // The bed's primary chnl is assigned.
    // Get the assigned scChnlDex. (This could be null if it has never been assigned.)
    $scChnlDex = $theBeds[$bKey][$bcKey]['assignedChnlDex'];//
    if($scChnlDex == null) {          // Has the 1st bed chnl been assigned?
      return -1;                      // No, let the caller know.
    }                                 // The 1st bed chnl has been assigned.
    return $theBeds[$bKey][$bcKey]['assignedChnlDex'];// Return the assigned scChnlDex.
  }                                   // This line should never be reached.
} // End of function getPrimaryBedChnlDex().

function timeDifString() {            // Returns the amount of time since the PM last updated in a string.
  $pmSettings = \App\PMSetting::find(1);// Open the pmSettings table to get the last time written by the PM.
  $dbRunTime = $pmSettings->runTime;  // Get the last time written to the DB by the PM.
  // Create a carbon time for determining the difference in time.
  $cRunTime = Carbon::createFromFormat('Y-m-d H:i:s', $dbRunTime, 'UTC');
  $cNowTime = Carbon::now('UTC');   // Get the UTC time for right now.
  // Get the difference in time from right now to last time written by the PM.
  $difSeconds = $cNowTime->diffInSeconds($cRunTime);
  $difMinutes = intdiv($difSeconds, 60);// Get the number of minutes for display purposes.
  $secondOfMinute = $difSeconds - $difMinutes * 60;// Calculate the second of the minute for display purposes.
  $strTime = $difMinutes . ' minutes, ' . $secondOfMinute ." seconds";// Create a time diff display string.
  return $strTime;
}

function getSessionConfig() {
  $theChnls = Chnl::all();          // Access the DB channels.
  $relays = Relay::all();           // Get the DB relays.
  $peLS = array();                  // This array contains most configuration settings for this session.
  $value = session('peLocalStorage', 'unknown');// It is unknown if the user allowed local storage.
  $peLS['isUsed'] = $value;         // isUsed is either unknown, true, or false based on if the user allows local storage.
  $colors = session('PE_Chnl_Color', 'undefined');// Get this sessions chnl colors.
  if($colors != 'undefined') {      // Are there session colors?
    $peLS['PE_Chnl_Color'] = $colors;// Yes. Set them.
  } else {                          // No.
    $chnlColors = array();          // Create an array for the chnl colors.
    for($chnlDex = 0; $chnlDex < sizeof($theChnls); $chnlDex++) {// Loop through each chnl.
      array_push($chnlColors, "#000000");// Set each chnl to the default color (black).
    } // End of looping through each chnl.
    $peLS['PE_Chnl_Color'] = $chnlColors; // Add the colors to the session variable.
  } // End of condition: Set each chnl's color.
  $chnlPlot = session('PE_Chnl_Plot', 'undefined');// Get this session's chnls to plot.
  if($chnlPlot != 'undefined') {      // Are there session chnls to plot?
    $peLS['PE_Chnl_Plot'] = $chnlPlot;// Yes. Set them.
  } else {                          // No.
    $chnlPlot = array();            // Create an array of flags to indicate if the chnl should be plotted.
    for($chnlDex = 0; $chnlDex < sizeof($theChnls); $chnlDex++) {// Loop through each chnl.
      array_push($chnlPlot, 0);     // Set each chnl not to plot.
    } // End of looping through each chnl.
    $peLS['PE_Chnl_Plot'] = $chnlPlot;   // Add the chnls to plot to the session variable.
  } // End of condition: Set each chnl's plot status.
  $colors = session('PE_Prgm_Color', 'undefined');// Get this sessions chnl colors.
  if($colors != 'undefined') {      // Are there session colors?
    $peLS['PE_Prgm_Color'] = $colors;// Yes. Set them.
  } else {                          // No.
    $prgmColors = array();          // Create an array for the program colors.
    for($chnlDex = 0; $chnlDex < sizeof($theChnls); $chnlDex++) {// Loop through each chnl.
      array_push($prgmColors, "#000000");// Set each program color to the default color (black).
    } // End of looping through each chnl.
    $peLS['PE_Prgm_Color'] = $prgmColors; // Add the program colors to the session variable.
  } // End of condition: Set each chnl's color.
  $prgmPlot = session('PE_Prgm_Plot', 'undefined');// Get this session's programs to plot.
  if($prgmPlot != 'undefined') {    // Are there session chnls to plot?
    $peLS['PE_Prgm_Plot'] = $prgmPlot;// Yes. Set them.
  } else {                          // No.
    $prgmPlot = array();            // Create an array of flags to indicate if the program should be plotted.
    for($chnlDex = 0; $chnlDex < sizeof($theChnls); $chnlDex++) {// Loop through each chnl.
      array_push($prgmPlot, 0);     // Set each program not to plot.
    } // End of looping through each chnl.
    $peLS['PE_Prgm_Plot'] = $prgmPlot;// Add the programs to plot to the session variable.
  } // End of condition: Set each program's plot status.
  $theTimeZone = \App\Configuration::find(1)->timeZone;// Get the time zone.
  $dt = new DateTime(null, new DateTimeZone($theTimeZone));// Create a new DateTime in the local time zone.
  $dt = $dt->format("Y-m-d H");     // Create the local start time accurate to the hour.
  $dt .= ":00";                     // Set the minutes to 00. The graph starts on an hour.
  // Each of the following session calls either retreives the existing variable or set the value to a default.
  $peLS['PE_TVT_StartTime'] = session('PE_TVT_StartTime', $dt);// Set the TVT start time.
  $peLS['PE_TVT_Span'] = session('PE_TVT_Span', 24);// Set TVT to plot 24 hours.
  $peLS['PE_TVT_Max'] = session('PE_TVT_Max', 180);// Set the TVT upper temperature grid.
  $peLS['PE_TVT_Min'] = session('PE_TVT_Min', 0);// Set the TVT lower temperature grid.
  $peLS['PE_TVT_Units'] = session('PE_TVT_Units', 'F');// Set the TVT temperature units.
  $peLS['PE_Relay_StartTime'] = session('PE_Relay_StartTime', $dt);// Set the relay history start time.
  $peLS['PE_Relay_Span'] = session('PE_Relay_Span', 24);// Set relay history to plot 24 hours.
  $colors = session('PE_Relay_Color', 'undefined');// Get this session's relay colors.
  if($colors != 'undefined') {      // Are there session relay colors?
    for($relayDex = 0; $relayDex < sizeof($relays); $relayDex++) {// Loop through each relay.
      if($colors[$relayDex] == 'undefined') {// Is this relay's color 'undefined'?
        $colors[$relayDex] = "#000000";// Yes. Set its color to black.
      } // End of condition: This relay has an undefined color.
    } // End of looping through each relay.
    $peLS['PE_Relay_Color'] = $colors;// Set the session's relay colors.
  } else {                          // No session relay colors.
    $relayColors = array();         // Create an array for the relay colors.
    for($relayDex = 0; $relayDex < sizeof($relays); $relayDex++) {// Loop through each relay.
      array_push($relayColors, "#000000");// Set each relay's color to the default color (black).
    } // End of looping through each relay.
    $peLS['PE_Relay_Color'] = $relayColors;// Add the relay colors to the session variable.
  } // End of condition: Set each relay's color.
  $relayPlot = session('PE_Relay_Plot', 'undefined');// Get this session's relays to plot.
  if($relayPlot != 'undefined') {      // Are there session relays to plot?
    $peLS['PE_Relay_Plot'] = $relayPlot;// Yes. Set them.
  } else {                          // No.
    $relayPlot = array();           // Create an array of flags to indicate if the relay should be plotted.
    for($relayDex = 0; $relayDex < sizeof($relays); $relayDex++) {// Loop through each relay.
      array_push($relayPlot, 0);    // Set each relay not to plot.
    } // End of looping through each relay.
    $peLS['PE_Relay_Plot'] = $relayPlot;// Add the relays to plot to the session variable.
  } // End of condition: Set each relay's plot status.
  // Future work: add additional variables if there are any.
  return $peLS;                     // Return the session variable.
} // End of function getSessionConfig.

function setSessionConfig($peLS) {    // Set the session configuration.
  session(['peLocalStorage'  => $peLS['isUsed']]);// Store session the local storage state.
  session(['PE_Chnl_Color'   => $peLS['PE_Chnl_Color']]);// Store the session chnl colors.
  session(['PE_Chnl_Plot'    => $peLS['PE_Chnl_Plot']]);// Store the session chnl plot flags.
  session(['PE_Prgm_Color'   => $peLS['PE_Prgm_Color']]);// Store the session program colors.
  session(['PE_Prgm_Plot'    => $peLS['PE_Prgm_Plot']]);// Store the session program plot flags.
  session(['PE_TVT_StartTime' => $peLS['PE_TVT_StartTime']]);// Store the session TVT start time.
  session(['PE_TVT_Span'     => $peLS['PE_TVT_Span']]);// Store the session TVT span.
  session(['PE_TVT_Max'      => $peLS['PE_TVT_Max']]);// Store the session TVT maximum temperature.
  session(['PE_TVT_Min'      => $peLS['PE_TVT_Min']]);// Store the session TVT minimum temperature.
  session(['PE_TVT_Units'    => $peLS['PE_TVT_Units']]);// Store the session TVT temperature units.
  session(['PE_Relay_StartTime'  => $peLS['PE_Relay_StartTime']]);// Store the session relay history start time.
  session(['PE_Relay_Span'   => $peLS['PE_Relay_Span']]);// Store the session relay history span.
  session(['PE_Relay_Color'  => $peLS['PE_Relay_Color']]);// Store the session relay colors.
  session(['PE_Relay_Plot'   => $peLS['PE_Relay_Plot']]);// Store the session relay history plot flags.
} // End of function setSessionConfig.

function m_log($arMsg)
{
  $theTimeZone = \App\Configuration::find(1)->timeZone;// Get the time zone.
  date_default_timezone_set($theTimeZone);
	//define empty string
	$stEntry="";
	//get the event occur date time,when it will happened
	$arLogData['event_datetime']='['.date('Y-m-d h:i:s A').'] [client '.$_SERVER['REMOTE_ADDR'].']';
	//if message is array type
	if(is_array($arMsg))
	{
  	//concatenate msg with datetime
  	foreach($arMsg as $msg)
    {
      if(is_array($msg))
    	{
        foreach($msg as $subMsg)
        {
          $stEntry.=$arLogData['event_datetime']." ".$subMsg."\n";
        }
      }
      else
      {
        $stEntry.=$arLogData['event_datetime']." ".$msg."\n";
      }
    }
  }
  else
  {   //concatenate msg with datetime
  	$stEntry.=$arLogData['event_datetime']." ".$arMsg."\n";
  }
  //create file with current date name
  $stCurLogFileName='log_'.date('Ymd').'.txt';
  //open the file append mode,dats the log file will create day wise
  $fHandler=fopen('/Users/Shared/Sites/bdat/'.$stCurLogFileName,'a+');
  //write the info into the file
  fwrite($fHandler,$stEntry);
  //close handler
  fclose($fHandler);
}

function my_alert($message)
{
  echo "<script>alert('$message');</script>";
}
