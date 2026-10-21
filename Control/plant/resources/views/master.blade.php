

<html>
<head>
	<title> @yield('title') </title>

	<!-- Material Design fonts -->
	<!--
	<link rel="stylesheet" type="text/css" href="//fonts.googleapis.com/css?family=Roboto:300,400,500,700">
	<link rel="stylesheet" type="text/css" href="//fonts.googleapis.com/icon?family=Material+Icons">
-->
	<!-- Bootstrap -->
	<!-- <link rel="stylesheet" type="text/css" href="//maxcdn.bootstrapcdn.com/bootstrap/3.3.7/css/bootstrap.min.css"> -->
	<!-- Bootstrap Material Design -->


	<link rel="stylesheet" type="text/css" href="/css/bootstrap-material-design.css">
	<link rel="stylesheet" type="text/css" href="/css/ripples.min.css">
	<link rel="stylesheet" type="text/css" href="/css/bootstrap.min.css">


	<!-- <link rel="stylesheet" type="text/css" href="/css/bootstrap-datetimepicker.min.css"> -->
<!--	<link rel="stylesheet" type="text/css" href="/css/ripples.min.css"> -->
	<script src="/js/jquery-3.3.1.min.js"></script>

</head>

<body>

<!-- Put an at symbol like the line below first. include('shared.navbar') -->

@include('topBar')

@yield('content')

<!--<script src="https://code.jquery.com/jquery-3.2.1.min.js"></script>-->
<!--<script src="https://maxcdn.bootstrapcdn.com/bootstrap/3.3.7/js/bootstrap.min.js"></script>-->
<script src="/js/bootstrap.min.js"></script>
<!-- <script src="/js/ripples.min.js"></script> -->
<script src="/js/material.min.js"></script>
<script>
	$(document).ready(function() {
		// The currently active item on the navbar should have a different backgroundColor.
		// This is accomplished by causing the active item to use the "active" class.
		// To start, the "active" class is removed from whichever item is using it.
		$('a.active').removeClass('active');
		// location.pathname returns a string containing the path portion of our URL.
		// Using location.pathname, the item which is active is located.
		// That item is then given the "active" class.
		$('a[href$="' + location.pathname + '"]').addClass('active');
		// This command is used to initialize some elements and make them work properly.
		$.material.init();
	});
</script>


</body>
</html>
