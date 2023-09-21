#pragma once

class Error
{
public:
	Error();
	~Error();

protected:
	void _400badRequest();
	void _401unauthorized();
	void _403forbidden();
	void _404notFound();
	void _405methodNotAllowed();
	void _406notAcceptable();
	void _408requestTimeout();
	void _409conflict();
	void _413requestEntityTooLarge();
	void _414uriTooLong();
	void _500internalServerError();
	void _501badGateWay();
	void _503serviceUnavailable();
	void _504gatewayTimeout();
	void _505httpVersionNotSupported();
};
