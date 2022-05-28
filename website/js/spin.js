// thorium.svg spinner

$( ".spin" ).click(function() {
    //alert($( this ).css( "transform" ));
    if (  $( this ).css( "transform" ) == 'none' ){
        $(this).css("transform","rotate(-360deg)");
    } else {
        $(this).css("transform","" );
    }
});
