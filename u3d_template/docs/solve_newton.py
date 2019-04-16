
import math

def solve( x0, f, df, eps=1.0e-6, maxN=16, e=1.7, M=123.0 ):
    x = x0
    xRes = x
    e = abs( f(x,e,M) )
    n = 0
    while ( e > eps ) and (n <= maxN):
        alpha = 1.0
        e1 = e
        f_fd = f(x,e,M) / df(x,e)
        x1 = x - f_fd*alpha
        e1 = abs(f(x1,e,M))
        if ( e1 >= e ):
            alpha *= 0.5
        else:
            xRes = x1
        x = x1
        n += 1
        e = e1
        print( "xn: {}".format( x ) )
    print( "result: {}".format( xRes ) )
    return e



#def f(x, e, M):
#    res = e*x*x - x - M
    #res = e/2.0*(x - 1.0/x) - math.log(x) - M
#    return res

#def df(x, e):
#    res = 2.0*e*x - 1.0
    #res = e*math.cosh(x) - 1.0;
    #res = e/2.0*(1.0 + 1.0/(x*x) ) - 1.0/x
#    return res;



def f(x):
    #return 6*x**5-5*x**4-4*x**3+3*x**2
    #return 1.7*x*x - x - 123.0   
    #return 1.7*math.sinh(x) - x - 123.0
    return 1.7/2.0*(x - 1/x) - math.log(x) - 123.0
 
def df(x):
    #return 30*x**4-20*x**3-12*x**2+6*x
    #return 1.7*2.0*x - 1.0
    #return 1.7*math.cosh(x) - 1.0
    return 1.7/2.0*(1 + 1/(x*x)) - 1/x
 
def dx(f, x):
    return abs(0-f(x))

def newtons_method(f, df, x0, e):
    delta = dx(f, x0)
    while delta > e:
        x0 = x0 - f(x0)/df(x0)
        delta = dx(f, x0)
    print( 'Root is at: ', x0 )
    print( 'f(x) at root is: ', f(x0) )

if __name__ == "__main__":
    #v = solve( 2.0, f, df )
    newtons_method(f, df, 1.0, 1e-5)
    print( "done" )



