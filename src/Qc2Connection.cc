
#include "Qc2Connection.h"

#include "Qc2App.h"
#include <milog/milog.h>

///Handles the db connection and disconnection after timeout

ConnectionHandler::ConnectionHandler( Qc2App & app )
    : app_( app )
    , con( 0 )
    , idleTime( 0 )
{
}

ConnectionHandler::~ConnectionHandler()
{
    LOGDEBUG( "Closing the database connection before termination!" );
    if ( con )
        app_.releaseDbConnection( con );
}
      
dnmi::db::Connection* ConnectionHandler::getConnection()
{
    if( !con ) {
        while ( !app_.shutdown() ) {
            con = app_.getNewDbConnection();
            if ( con ) {
                LOGDEBUG( "Created a new connection to the database!" );
                break;
            }
            LOGINFO( "Can't create a connection to the database, retry in 5 seconds .." );
            sleep( 5 );
        }
    }
    idleTime = 0;
    return con;
}
      
void ConnectionHandler::notNeeded()
{
    if ( con and ++ idleTime >= max_idle_time ) {
        LOGDEBUG( "Closing the database connection!" );
        app_.releaseDbConnection( con );
        con = 0;
        idleTime = 0;
    }
}
