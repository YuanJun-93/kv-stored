# kv-stored
C++实现的kv存储

为了方便key的比较, key默认为int类型, 如果是其他类型需要自己实现比较器

目前只使用跳表作为基础数据结构, 跳表头结点的key默认设为-1

**跳表的插入、删除和查找时间复杂度为O(logN)**, 极端情况下(单链表, 只有一层)的时间复杂度是O(N)

作为扩展, 还有一个set操作没有写, 想尝试同学可以自己写一下set操作, 最简单的方式是del+put

SkipList基础功能：
1. put(key, value)
    - 插入一个key到跳表中
    - key已存在跳表中会插入失败返回-1, 插入成功返回1
2. del(key)
    - 从跳表中删除该key
    - key不存在返回-1, 删除成功返回1
3. get(key)
    - 从跳表中获取获取key的value
    - key不存在返回-1, key存在返回对应的value
4. save_data()
    - 保存当前跳表的元素到当前目录的data.txt文件中
5. load_data()
    - 加载当前路径下data.txt文件
    - 如果当前跳表中的元素和data.txt中的key重复, data.txt会跳过该元素的加载, 对跳表没有任何影响

