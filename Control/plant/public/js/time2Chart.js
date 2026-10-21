var ctx = document.getElementById("myChart").getContext('2d');


var hour = startMoment.get('hour');

var dayOfYear = startMoment.dayOfYear();

alert("setting.max: " + setting.max);
var maxTemp = Number(setting.max);
var minTemp = Number(setting.min)

var month = startMoment.month() + 1;
if(month < 10) {
  month = "0" + month;
}

var day = startMoment.date();
if(day < 10) {
  day = "0" + day;
}

var yLabel = "Temperature (F)";
if(setting.units == "C") {
  yLabel = "Temperature (C)";
}

//var ttSpan = jQuery('#hourSpan').val();
//var ttSpan = 16;

//var endMoment = startMoment.clone().add(ttSpan, 'hours');
var endMoment = startMoment.clone().add(setting.span, 'hours');

var xLabel = "Time starting on " + startMoment.year() + "-" + month + "-" + day + " at " + hour + ":00";

var chnlObjs = [];                    // An array of items to plot. Includes all display settings and the time/temperatures.
var plotChnlDex = 0;                  // The index of the time/temperature array in "allTvtData" to plot.
                                      // "allTvtData" contains x and y coordinates (time and temperature coordivates) for
                                      // each channel that is to be plotted. Thus, allTvtData[0] contains all the
                                      // time/temperature coordinates for the first channel to plot. This may not be the
                                      // channel with index 0. allTvtData[1] contains all the time/temperature coordinates for
                                      // the second channel to plot. etc...
for(var nDex = 0; nDex < allChnls.length; nDex++) {
  if(allChnls[nDex].plotTemperature == 0) {
    continue;
  }
  var colorStr = "hsl(" + allChnls[nDex].colorHue + "," + allChnls[nDex].colorSaturation;
  colorStr += "%," + allChnls[nDex].colorLightness + "%)";
  chnlPlotSettings = {
    label: allChnls[nDex].name,
    fill: false,
    lineTension: 0.1,
    backgroundColor: colorStr,
    borderColor: colorStr,
    borderCapStyle: 'butt',
    borderDash: [],
    borderDashOffset: 0.0,
    borderJoinStyle: 'miter',
    pointBorderColor: colorStr,
    pointBackgroundColor: colorStr,
    pointBorderWidth: 1,
    pointHoverRadius: 5,
    pointHHoverBackgroundColor: "rgba(75,192,192,1)",
    pointHoverBorderColor: "rgba(220,220,220,1)",
    pointHoverBorderWidth: 2,
    pointRadius: 1,
    pointHitRadius: 10,
    data: allTvtData[plotChnlDex],
  }
  chnlObjs.push(chnlPlotSettings);
  plotChnlDex++;
}

// Build the objects for jsChart to plot programs.
for(var nDex = 0; nDex < allChnls.length; nDex++) {
  if(allChnls[nDex].plotProgram == 0) {
    continue;
  }
  var colorStr = "hsl(" + allChnls[nDex].prgHue + "," + allChnls[nDex].prgSaturation;
  colorStr += "%," + allChnls[nDex].prgLightness + "%)";
  prgPlotSettings = {
    label: "P-" + allChnls[nDex].name,
    fill: false,
    lineTension: 0.0,
    backgroundColor: colorStr,
    borderColor: colorStr,
    borderCapStyle: 'butt',
    borderDash: [],
    borderDashOffset: 0.0,
    borderJoinStyle: 'miter',
    pointBorderColor: colorStr,
    pointBackgroundColor: colorStr,
    pointBorderWidth: 1,
    pointHoverRadius: 5,
    pointHHoverBackgroundColor: "rgba(75,192,192,1)",
    pointHoverBorderColor: "rgba(220,220,220,1)",
    pointHoverBorderWidth: 2,
    pointRadius: 1,
    pointHitRadius: 10,
    data: allTvtData[plotChnlDex],
  }
  chnlObjs.push(prgPlotSettings);
  plotChnlDex++;
}


var myChart = new Chart(ctx, {
    type: 'line',
    data: {
//        datasets: [
//          chnlObjs
//        ]
        datasets:
          chnlObjs
    },
    options: {
      maintainAspectRatio: true,
        scales: {
          xAxes: [{
						type: "time",
						display: true,
						scaleLabel: {
							display: true,
							labelString: xLabel,
						},
            time: {
              min: startMoment,
              max: endMoment
            }
					}],
            yAxes: [{
                scaleLabel: {
    							display: true,
    							labelString: yLabel,
    						},
                ticks: {
                    max: maxTemp,
                    min: minTemp,
                }
            }]
        }
    }

});
