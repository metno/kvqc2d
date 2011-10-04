// -*- c++ -*-

#ifndef _Qc2Connection_h
#define _Qc2Connection_h

class Qc2App;
namespace dnmi {
namespace db {
class Connection;
} // namespace db
} // namespace dnmi
  
/// Handles the db connection and disconnection after timeout

class ConnectionHandler {
    Qc2App & app_;
    dnmi::db::Connection * con;
    int idleTime;
    static const int max_idle_time = 60;

public:
    ConnectionHandler( Qc2App & app );
    ~ConnectionHandler();

    /// returns a connection to db, either freshly generated or an old one
    dnmi::db::Connection* getConnection();
      
    /// Signal that the db connection is not needed. If this signal is used
    /// \c max_idle_time times in a row, the db connection will be released.
    void notNeeded();
};

// class Qc2Connection {
// 
// public:
//     Qc2Connection( dnmi::db::Connection *con );
// 
//     bool dbOk() const
//         { return connection_ok; }
// 
//     dnmi::db::Connection* getConnection()
//         { return connection_; }
// 
// private:
//     bool connection_ok;
//     kvalobs::kvDbGate dbGate;
// 
//     dnmi::db::Connection * connection_;
// };

#endif
