#include "webpage.cpp"

const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html>
<head>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <style>
    html {
     font-family: Arial;
     display: inline-block;
     margin: 0px auto;
     text-align: center;
    }

    h2 { font-size: 3.0rem; }

    p { font-size: 3.0rem; }
    .units { font-size: 1.2rem; }

    .sensor-labels{
      font-size: 1.5rem;
      vertical-align:middle;
      padding-bottom: 15px;
    }
  </style>
</head>

<body>
  <h2>Brake temp. monitor</h2>

  <p>
    <span class="sensor-labels">Front</span> 
    <span id="TEMPF">%TEMPF%</span>
    <sup class="units">&deg;C</sup>
  </p>

  <p>
    <span class="sensor-labels">Rear</span>
    <span id="TEMPR">%TEMPR%</span>
    <sup class="units">&deg;C</sup>
  </p>

  <form method="get" action="file.doc">
    <button type="submit">Download!</button>
  </form>
</body>

<script>
setInterval(function ( ) {
  var xhttp = new XMLHttpRequest();

  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById("TEMPF").innerHTML = this.responseText;
    }
  };

  xhttp.open("GET", "/TEMPF", true);
  xhttp.send();

}, 10000 ) ;

setInterval(function ( ) {
  var xhttp = new XMLHttpRequest();

  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById("TEMPR").innerHTML = this.responseText;
    }
  };

  xhttp.open("GET", "/TEMPR", true);
  xhttp.send();

}, 10000 ) ;

</script>
</html>
)rawliteral";
