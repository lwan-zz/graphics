drawSubTree( depth )
{
    if( depth == 0 ) return;
     
    // draw some geometry for the current edge
    rect( x, y, w, h );
    
    // apply some transformations
    rotate( theta );    
    translate( u, v );  
    scale( s );         
    rect( x, y, w, h ); 
   
    drawSubTree( depth-1 );
 
    // apply some transformations

    drawSubTree( depth-1 );
 
    // apply some transformations
 }