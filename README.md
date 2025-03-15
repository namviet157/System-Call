# HỆ ĐIỀU HÀNH

# PROJECT 2 – System call

Bài thực hành này sẽ giúp bạn tìm hiểu cách thêm mới một system call vào xv6\

## 1. Trace
Trong bài tập này, bạn sẽ thêm một tính năng theo dõi lệnh gọi hệ thống có thể giúp bạn khi debug các bài lab sau. 
Bạn sẽ tạo một system call mới là *trace*  để điều khiển theo dõi. 
Lệnh này sẽ lấy một  đối số, một số nguyên "mask", có các bit chỉ định các system call nào cần theo dõi.\
Ví dụ: Để theo dõi lệnh gọi hệ thống fork, một chương trình sẽ gọi 
  ```php
  trace(1 << SYS_fork)
```
trong đó *SYS_fork* là mã số syscall được định nghĩa từ *kernel/syscall.h* . 
Bạn phải sửa đổi kernel xv6 để in ra mỗi dòng thông tin khi có system call bị theo dõi được gọi, (nghĩa là mã số system call được đặt trong mask) 
lúc này sẽ in ra thông tin gồm: tên system call được gọi và id của tiến trình gọi system call này, bạn không cần phải in các đối số của system call. 
System call *trace* sẽ bật theo dõi cho tiến trình gọi nó và bất kỳ tiến trình con nào mà nó phân nhánh sau đó, nhưng không được ảnh hưởng đến các quy trình khác.\

Viết một chương trình trace cấp độ người dùng chạy một chương trình khác với chức năng theo dõi được bật (xem user/trace.c ). Khi hoàn tất, bạn sẽ thấy đầu ra như thế này:
```php
$ trace 32 grep hello README
3: syscall read -> 1023
3: syscall read -> 966
3: syscall read -> 70
3: syscall read -> 0
$
$ trace 2147483647 grep hello README
4: syscall trace -> 0
4: syscall exec -> 3
4: syscall open -> 3
4: syscall read -> 1023
4: syscall read -> 966
4: syscall read -> 70
4: syscall read -> 0
4: syscall close -> 0
$
$ grep hello README
$
$ trace 2 usertests forkforkfork
usertests starting
test forkforkfork: 407: syscall fork -> 408
408: syscall fork -> 409
409: syscall fork -> 410
410: syscall fork -> 411
409: syscall fork -> 412
410: syscall fork -> 413
409: syscall fork -> 414
411: syscall fork -> 415
...
$
```

Trong ví dụ đầu tiên ở trên, *trace* gọi lệnh *grep* theo dõi chỉ lệnh gọi hệ thống *read*. 32 là *1<<SYS_read* .\
Trong ví dụ thứ hai, trace chạy lệnh grep trong khi theo dõi tất cả các lệnh gọi hệ thống; 2147483647 có tất cả 31 bit thấp được đặt. \
Trong ví dụ thứ ba, chương trình không được theo dõi, do đó không có đầu ra trace nào được in ra. \
Trong ví dụ thứ tư, các lệnh gọi hệ thống fork của tất cả các con cháu của bài kiểm tra *forkforkfork* trong *usertests* đang được theo dõi. 
Giải pháp của bạn là đúng nếu chương trình của bạn hoạt động như được hiển thị ở trên (mặc dù ID quy trình có thể khác).
### Một số gợi ý:
- Thêm *$U/_trace* vào *UPROGS* trong *Makefile*
- Chạy *make qemu* và bạn sẽ thấy trình biên dịch không thể biên dịch *user/trace.c* , vì các stub không gian người dùng cho lệnh gọi hệ thống *theo dõi* vẫn chưa tồn tại: thêm một nguyên mẫu cho trace vào *user/user.h* ,
một stub vào *user/usys.pl* và một số syscall vào *kernel/syscall.h* . Makefile gọi tập lệnh perl *user/usys.pl* , tạo ra *user/usys.S* , các stub lệnh gọi hệ thống thực tế, sử dụng lệnh RISC-V *ecall* để
chuyển sang kernel. Sau khi bạn sửa các sự cố biên dịch, hãy chạy *trace 32 grep hello README*; nó sẽ không thành công vì bạn vẫn chưa triển khai lệnh gọi hệ thống trong kernel.
- Thêm hàm *sys_trace(*) trong *kernel/sysproc.c* để triển khai lệnh gọi hệ thống mới bằng cách ghi nhớ đối số của lệnh trong một biến mới trong cấu trúc proc (xem *kernel/proc.h* ).
Các hàm để truy xuất đối số lệnh gọi hệ thống từ không gian người dùng nằm trong *kernel/syscall.c* và bạn có thể xem ví dụ về cách sử dụng chúng trong *kernel/sysproc.c* .
Thêm *sys_trace* mới của bạn vào mảng syscalls trong *kernel/syscall.c*.
- Sửa đổi *fork()* (xem *kernel/proc.c* ) để sao chép mặt nạ theo dõi từ tiến trình cha sang tiến trình con.
- Sửa đổi hàm *syscall()* trong *kernel/syscall.c* để in ra kết quả theo dõi. Bạn sẽ cần thêm một mảng tên syscall để lập chỉ mục vào.

## 2. Sysinfo
Trong bài tập này, bạn sẽ thêm một system call mới là *sysinfo* , để thu thập thông tin về thông tin system đang chạy. 
Lệnh gọi hệ thống này lấy một đối số: là con trỏ *struct sysinfo* (xem *kernel/sysinfo.h* ). Kernel sẽ điền vào các trường của struct này: trường *freemem* sẽ được đặt thành số byte bộ nhớ trống và 
trường *nproc* sẽ được đặt thành số tiến trình có *trạng thái* không phải là *UNUSED* . Viết một chương trình thử nghiệm *sysinfotest* ; bạn sẽ vượt qua bài tập này nếu nó in ra *"sysinfotest: OK"*.\
### Một số gợi ý:
- Thêm *$U/_sysinfotest* vào *UPROGS* trong *Makefile*
- Chạy *make qemu*; *user/sysinfotest.c* sẽ không biên dịch được. Thêm system call sysinfo, thực hiện theo các bước tương tự như trong bài tập trước.
Để khai báo nguyên mẫu cho sysinfo() trong *user/user.h*, bạn cần khai báo trước sự tồn tại của *struct sysinfo* :
```php
    struct sysinfo;
    int sysinfo(struct sysinfo *);
```
  
Sau khi khắc phục được sự cố biên dịch, hãy chạy *sysinfotest*; lệnh này sẽ không thành công vì bạn chưa triển khai system call trong kernel.
- *sysinfo* cần sao chép *struct sysinfo* ra *user space*;  xem *sys_fstat()* ( *kernel/sysfile.c* ) và *filestat()* ( *kernel/file.c* ) để biết ví dụ về cách thực hiện việc đó bằng cách sử dụng *copyout()* .
- Để thu hồi bộ nhớ trống, hãy thêm một hàm vào *kernel/kalloc.c*
- Để thu hồi tiến trình, hãy thêm một hàm vào *kernel/proc.c*

