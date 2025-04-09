
#pragma once

namespace mcs::net::common
{
    /**
     * @brief Syntax Notation:
     * https://www.rfc-editor.org/rfc/rfc3986.html#:~:text=scheme%3A..%22.%0A%0A1.3.-,Syntax%20Notation,-This%20specification%20uses
     * @see[ABNF]:
     * https://www.rfc-editor.org/rfc/rfc3986.html#appendix-A:~:text=Appendix%20A.%20%20Collected%20ABNF%20for%20URI
     * URI           = scheme ":" hier-part [ "?" query ] [ "#" fragment ]
     * scheme        = ALPHA *( ALPHA / DIGIT / "+" / "-" / "." )
     *  hier-part     = "//" authority path-abempty
                    / path-absolute
                    / path-rootless
                    / path-empty
     */
    /**
    hier-part: 解析
    类型	                    前缀    是否含authority	路径是否可为空	示例
    //authority path-abempty	//	    是	           是           http://example.com/foo
    path-absolute	            /	    否	           否（至少 /）	/foo/bar
    path-rootless	            无	    否	           是	        foo/bar
    path-empty	                无	    否	           必须为空	     http://example.com
*/
    struct URI
    {
    };
}; // namespace mcs::net::common