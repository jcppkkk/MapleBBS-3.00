      program tran
      implicit none

      integer name,i,state
      parameter (name=501)
      character*8 string(name),string1(name)
      character*4 price(name),price1(name)
      character*60 s_title

      open(unit=10,file='/home/bbs/game/stock_now1',
     &     access='sequential',iostat=state,status='old')
      open(unit=20,file='/home/bbs/game/stock_now',
     &     access='sequential',iostat=state,status='old')

      read(10,'(22X,A8)') s_title
      write(20,'(A10,A1,A8,A1)') '  諸羅股市','(',s_title,')'
      write(20,*) '  1234567890 chiayi BBS 提供'

      do i=1,3,1
       read(10,*) s_title
       read(10,'(7X,A60)') s_title
       write(20,'(A60)') s_title
      enddo

      read(10,'(A60)') s_title
      write(20,'(A60)') s_title

      do i=1,name,1
       read(10,15) string(i),string1(i),price(i),price1(i)
       write(20,25) '|',string(i),string1(i),'|',
     &     price(i),price1(i),'|'
      enddo

15    format(A4,3X,A6,3X,A3,1X,A2,A1)
25    format(A1,A4,1X,A6,A1,4X,A3,A2,A1)
      close(10)
      close(20)

      stop
      end
