import sys

import geometry_msgs.msg
import rclpy

if sys.platform == 'win32':
    import msvcrt
else:
    import termios
    import tty
print("\ti\nj\tk\tl\t\n\nw/x zwiększa prędkość liniową\ne/c zwiększa prędkość kątową")

moveBindings = {
    'i': (1, 0, 1, 0),
    'j': (0, 0, 1, 1),
    'l': (0, 0, 1, -1),
    'k': (0, 0, 1, 0),
    ',': (-1, 0, 1, 0),
}

speedBindings = {
    'w': (1, 0, 0),
    'x': (-1, 0, 0),
    'e': (0, 1, 0),
    'c': (0, -1, 0),
    'r': (0, 0, 1),
    'v': (0, 0, -1),
}


def getKey(settings):
    if sys.platform == 'win32':
        # getwch() returns a string on Windows
        key = msvcrt.getwch()
    else:
        tty.setraw(sys.stdin.fileno())
        # sys.stdin.read() returns a string on Linux
        key = sys.stdin.read(1)
        termios.tcsetattr(sys.stdin, termios.TCSADRAIN, settings)
    return key


def saveTerminalSettings():
    if sys.platform == 'win32':
        return None
    return termios.tcgetattr(sys.stdin)


def restoreTerminalSettings(old_settings):
    if sys.platform == 'win32':
        return
    termios.tcsetattr(sys.stdin, termios.TCSADRAIN, old_settings)


def vels(speed, turn, control_vel):
    return 'aktualnie:\tpredkosc liniowa: %s\tpredkosc kątowa:  %s\tbieg:  %s' % (speed, turn, control_vel)


def main():
    settings = saveTerminalSettings()

    rclpy.init()

    node = rclpy.create_node('teleop_twist_keyboard')
    pub = node.create_publisher(geometry_msgs.msg.Twist, 'cmd_vel', 10)

    speed = 1.0
    turn = 1.0
    control_vel = 1.0
    x = 0.0
    y = 0.0
    z = 1.0
    th = 0.0
    status = 0.0

    try:
        print(vels(speed, turn, control_vel))
        while True:
            key = getKey(settings)
            if key in moveBindings.keys():
                x = moveBindings[key][0]
                y = moveBindings[key][1]
                z = moveBindings[key][2]
                th = moveBindings[key][3]
            elif key in speedBindings.keys():
                speed = speed + speedBindings[key][0]
                if(speed>=9.0):
                    speed=9.0
                if(speed<=1.0):
                    speed=1.0
                turn = turn + speedBindings[key][1]
                if(turn>=9.0):
                    turn=9.0
                if(turn<=1.0):
                    turn=1.0
                control_vel = control_vel + speedBindings[key][2]
                if(control_vel>=2.0):
                    control_vel=2.0
                if(control_vel<=1.0):
                    control_vel=1.0
                print(vels(speed, turn, control_vel))
                status = (status + 1) % 15
            else:
                if (key == '\x03'):
                    break
                continue
               
            twist = geometry_msgs.msg.Twist()
            twist.linear.x = x * speed
            twist.linear.y = y * speed
            twist.linear.z = z * control_vel 
            twist.angular.x = 0.0
            twist.angular.y = 0.0
            twist.angular.z = th * turn
            pub.publish(twist)

    except Exception as e:
        print(e)

    finally:
        twist = geometry_msgs.msg.Twist()
        twist.linear.x = 0.0
        twist.linear.y = 0.0
        twist.linear.z = 0.0
        twist.angular.x = 0.0
        twist.angular.y = 0.0
        twist.angular.z = 0.0
        pub.publish(twist)

        restoreTerminalSettings(settings)


if __name__ == '__main__':
    main()
