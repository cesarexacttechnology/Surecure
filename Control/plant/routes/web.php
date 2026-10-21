<?php

Auth::routes();
/*
|--------------------------------------------------------------------------
| Web Routes
|--------------------------------------------------------------------------
|
| Here is where you can register web routes for your application. These
| routes are loaded by the RouteServiceProvider within a group which
| contains the "web" middleware group. Now create something great!
|
*/

/*
Route::get('/', function () {
    return view('welcome');
});
*/

Route::get('home', 'PagesController@plant');
Route::post('home', 'PagesController@updatePlant');
Route::get('/', 'PagesController@plant');
Route::post('/', 'PagesController@updatePlant');
Route::get('plant', 'PagesController@plant');
Route::post('plant', 'PagesController@updatePlant');
Route::get('relayTest', 'PagesController@relayTest')->middleware('auth');
Route::post('relayTest', 'PagesController@turnOnRelay')->middleware('auth');
Route::get('plotTemps', 'PagesController@plotTemps');
Route::post('plotTemps', 'PagesController@updatePlotTemps');
Route::get('cure', 'PagesController@cure')->middleware('auth');
Route::post('cure', 'PagesController@updateCure')->middleware('auth');
Route::get('plotRelays', 'PagesController@plotRelays');
Route::post('plotRelays', 'PagesController@updatePlotRelays');
Route::get('beds', 'PagesController@beds')->middleware('auth');
Route::post('beds', 'PagesController@updateBeds')->middleware('auth');
Route::get('chnls', 'PagesController@chnls')->middleware('auth');
Route::post('chnls', 'PagesController@updateChnls')->middleware('auth');

Route::get('peNetwork', 'PagesController@peNetwork')->middleware('auth');
Route::post('peNetwork','PagesController@updatePENetwork')->middleware('auth');
Route::get('setSC2Pw','Auth\LoginController@setSC2Pw');
Route::get('login', [ 'as' => 'login', 'uses' => 'Auth\LoginController@login']);
Route::post('login', [ 'as' => 'login', 'uses' => 'Auth\LoginController@pwProcess']);

/*
Route::get('peNetwork', 'PagesController@peNetwork');
Route::post('peNetwork','PagesController@updatePENetwork');
Route::get('pw', 'Auth\LoginController@pw');
Route::post('pw','PagesController@peSetNetwork');
*/
