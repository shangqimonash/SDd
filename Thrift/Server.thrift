namespace cpp server

struct serialised_bid {
    1:string input,
    2:i32 count;
}

service SSEService {
    oneway void initialise(1:i32 index),
    bool exist(1:i32 instance, 2:i32 index),
    oneway void add(1:i32 instance, 2:i32 index, 3:string key_str, 4:string value_str),
    binary get(1:i32 instance, 2:i32 index, 3:i32 pos),
    string read_omap(1:i32 instance, 2: serialised_bid bid),
    oneway void put_omap(1:i32 instance, 2: serialised_bid bid, 3:string value),
    i32 move(1:i32 instance),
    i32 finalise(1:i32 instance),
    list<binary> search(1:i32 instance, 2:i32 index, 3:binary token)
}