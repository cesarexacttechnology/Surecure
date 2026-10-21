@extends('master')
@section('name', 'login')

@section('content')

<!doctype html>
<html>
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
  max-width: 500px;
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

@media (min-width: 300px) {
  form {
    grid-template-columns: 140px 1fr;
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

<body>

  <h1 class="pe_title">Login</h1>

  @if($message = Session::get('error'))
  <div class="alert alert-danger alert-block">
    <button type="button" class="close" data-dismiss="alert">x</button>
    <strong>{{ $message }}</strong>
  </div>
  @endif

  @if(count($errors) > 0)
    @foreach ($errors->all() as $error)
      <p class="alert alert-danger">{{ $error }}</p>
    @endforeach
  @endif

  <form id="pwForm" method="post">


    <!-- The next line submits the token required by Laravel -->
    <input type="hidden" name="_token" value="{!! csrf_token() !!}">

    <label for="userID">User ID:</label>
    <input type="text" id="userID" name="userID">
    <label for="pwTxt">Password:</label>
    <input type="text" id="pwTxt" name="pwTxt">
    <button class="peBtns" type="submit">Submit</button>
    <button class="peBtns" type="button" onclick="location.href='{{ url('/')}}'">Cancel</button>

  </form>
</body>

</html>
@endsection
