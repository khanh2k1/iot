// server gui data len client
function clock() {
  var today = new Date();

  var date = today.getDate() + "/" + (today.getMonth() + 1) + "/" + today.getFullYear();
  var time = today.getHours() + " : " + today.getMinutes() + " : " + today.getSeconds();

  document.getElementById("time").innerHTML = time;
  document.getElementById("date").innerHTML = date;
}

setInterval(clock, 1000);

var temperature = document.getElementById("temperature");
var dbRef = firebase.database().ref("temperature");
dbRef.on("value", (snap) => (temperature.innerText = snap.val()));

var humidity = document.getElementById("humidity");
var dbRef = firebase.database().ref("humidity");
dbRef.on("value", (snap) => (humidity.innerText = snap.val()));

var ppm = document.getElementById("ppm");
var dbRef = firebase.database().ref("ppm");
dbRef.on("value", (snap) => (ppm.innerText = snap.val()));

var temperature_safe = document.getElementById("temperature_safe");
var dbRef = firebase.database().ref("temperature-safe");
dbRef.on("value", (snap) => (temperature_slider.value = snap.val()));
dbRef.on("value", (snap) => (temp_value.innerText = snap.val()));

var humidity_safe = document.getElementById("humidity_safe");
var dbRef = firebase.database().ref("humidity-safe");
dbRef.on("value", (snap) => (humidity_slider.value = snap.val()));
dbRef.on("value", (snap) => (humidity_value.innerText = snap.val()));

var ppm_safe = document.getElementById("ppm_safe");
var dbRef = firebase.database().ref("ppm-safe");
dbRef.on("value", (snap) => (ppm_slider.value = snap.val()));
dbRef.on("value", (snap) => (ppm_value.innerText = snap.val()));

var dbRef = firebase.database().ref("warning");
var audio = new Audio("music/warning.mp3");

// trang thai canh bao
dbRef.on("value", (snap) => {
  if (snap.val() == true) {
    audio.play();
    $('.body').css('background-color', 'red')
    document.getElementById("ledResult").outerHTML =
      '<input type="checkbox" id="ledResult" checked>';
  }
  if (snap.val() == false) {
    audio.pause();
    $('.body').css('background-color', '#03a9f4')
    document.getElementById("ledResult").outerHTML =
      '<input type="checkbox" id="ledResult">';
  }
});


// tat canh bao bang tay (mode1)
var mode1 = document.getElementById("mode1");
var dbRef = firebase.database().ref("mode").child("off");

dbRef.on("value", (snap) => {
  if (snap.val() == true) {
    audio.pause();
    document.getElementById("mode1").outerHTML =
      '<input type="radio" id="mode1" name="mode" checked>';
  } else {
    document.getElementById("mode1").outerHTML =
      '<input type="radio" id="mode1" name="mode">';
  }
});

// mo canh bao bang tay (mode2)
var mode2 = document.getElementById("mode2");
var dbRef = firebase.database().ref("mode").child("on");

dbRef.on("value", (snap) => {
  if (snap.val() == true) {
    audio.play();
    document.getElementById("mode2").outerHTML =
      '<input type="radio" id="mode2" checked name="mode">';
  } else {
    document.getElementById("mode2").outerHTML =
      '<input type="radio" id="mode2" name="mode">';
  }
});

// tu dong canh bao (mode3)
var mode1 = document.getElementById("mode3");
var dbRef = firebase.database().ref("mode").child("auto");

dbRef.on("value", (snap) => {
  if (snap.val() == true) {
    document.getElementById("mode3").outerHTML =
      '<input type="radio" id="mode3" checked name="mode">';
  } else {
    document.getElementById("mode3").outerHTML =
      '<input type="radio" id="mode3" name="mode">';
  }
});

// client gui ve server


$(document).ready(function () {

  // nhiet do  
  $('#temperature_slider').change(function () {
    var temperature_slider = document.getElementById("temperature_slider");
    var temp_value = document.getElementById("temp_value");
    temp_value.innerHTML = temperature_slider.value;
    temperature_slider.oninput = function () {
      temp_value.innerHTML = this.value;
    }
    firebase.database().ref("temperature-safe").set(parseFloat(temperature_slider.value));
    alert("Nhiêt độ an toàn là : " + parseFloat(temperature_slider.value));
  });

  $('#humidity_slider').change(function () {
    var humidity_slider = document.getElementById("humidity_slider");
    var humidity_value = document.getElementById("humidity_value");
    humidity_value.innerHTML = humidity_slider.value;
    humidity_slider.oninput = function () {
      humidity_value.innerHTML = this.value;
    }
    firebase.database().ref("humidity-safe").set(parseFloat(humidity_slider.value));
    alert("Độ ẩm an toàn là : " + parseFloat(humidity_slider.value));
  });

  $('#ppm_slider').change(function () {
    var ppm_slider = document.getElementById("ppm_slider");
    var ppm_value = document.getElementById("ppm_value");
    ppm_value.innerHTML = ppm_slider.value;
    ppm_slider.oninput = function () {
      ppm_value.innerHTML = this.value;
    }
    firebase.database().ref("ppm-safe").set(parseFloat(ppm_slider.value));
    alert("Chất lượng không khí an toàn là : " + parseFloat(ppm_slider.value));
  });


  // var humidity_slider = document.getElementById("humidity_slider");
  // var humidity_value = document.getElementById("humidity_value");

  // humidity_value.innerHTML = slider.value;

  // humidity_slider.oninput = function () {
  //   humidity_value.innerHTML = this.value;
  // }

  // var ppm_slider = document.getElementById("ppm_slider");
  // var ppm_value = document.getElementById("ppm_value");
  // ppm_value.innerHTML = slider.value;
  // slider.oninput = function () {
  //   output.innerHTML = this.value;
  // }

  const alertStatus = (e) => {
    // tắt cảnh báo bằng tay
    if ($("#mode1").is(":checked")) {
      firebase.database().ref("mode").child("off").set(true);
      firebase.database().ref("mode").child("on").set(false);
      firebase.database().ref("mode").child("auto").set(false);
      firebase.database().ref("warning").set(false);

      document.getElementById("ledResult").outerHTML =
        '<input type="checkbox" id="ledResult">';
    }
    // mở cảnh báo bằng tay
    if ($("#mode2").is(":checked")) {
      firebase.database().ref("mode").child("on").set(true);
      firebase.database().ref("mode").child("off").set(false);
      firebase.database().ref("mode").child("auto").set(false);
      firebase.database().ref("warning").set(true);

      document.getElementById("ledResult").outerHTML =
        '<input type="checkbox" id="ledResult" checked>';
    }

    // mở cảnh báo tự động
    if ($("#mode3").is(":checked")) {
      firebase.database().ref("mode").child("auto").set(true);
      firebase.database().ref("mode").child("off").set(false);
      firebase.database().ref("mode").child("on").set(false);
    }
  };
  // Attaching the click event on the button
  $(document).on("click", "#mode1", alertStatus);
  $(document).on("click", "#mode2", alertStatus);
  $(document).on("click", "#mode3", alertStatus);
});

$(document).ready(function () {
  $("#home_page").click(function () {
    $("#home_page").css({ "font-size": "25px", color: "white" });
  });

  $("#safe_page").click(function () {
    $("#safe_page").css("font-size", "25px");
  });

  $("#btn_temperature").click(function () {
    var temperature_safe = document.getElementById("temperature_input").value;
    if (temperature_safe != "") {
      document.getElementById("temperature_safe").innerHTML = temperature_safe;
      firebase
        .database()
        .ref("temperature-safe")
        .set(parseFloat(temperature_safe));
    } else {
      alert("Bạn chưa nhập !");
      var temperature_safe = document.getElementById("temperature_safe");
      var dbRef = firebase.database().ref("temperature-safe");
      dbRef.on("value", (snap) => (temperature_safe.innerText = snap.val()));
    }
  });

  $("#btn_humidity").click(function () {
    var humidity_safe = document.getElementById("humidity_input").value;
    if (humidity_safe != "") {
      document.getElementById("humidity_safe").innerHTML = humidity_safe;
      firebase.database().ref("humidity-safe").set(parseInt(humidity_safe));
    } else {
      alert("Bạn chưa nhập !");
      var humidity_safe = document.getElementById("humidity_safe");
      var dbRef = firebase.database().ref("humidity-safe");
      dbRef.on("value", (snap) => (humidity_safe.innerText = snap.val()));
    }
  });

  $("#btn_ppm").click(function () {
    var ppm_safe = document.getElementById("ppm_input").value;
    if (ppm_safe != "") {
      document.getElementById("ppm_safe").innerHTML = ppm_safe;
      firebase.database().ref("ppm-safe").set(parseInt(ppm_safe));
    } else {
      alert("Bạn chưa nhập !");
      var ppm_safe = document.getElementById("ppm_safe");
      var dbRef = firebase.database().ref("ppm-safe");
      dbRef.on("value", (snap) => (ppm_safe.innerText = snap.val()));
    }
  });
});

